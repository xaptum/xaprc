/*
 * Copyright 2018 Xaptum, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <pcap.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define LogDebug(format, ...) syslog(LOG_DEBUG, format, ##__VA_ARGS__)
#define LogCrit(format, ...) syslog(LOG_CRIT, format, ##__VA_ARGS__)
#define LogInfo(format, ...) syslog(LOG_INFO, format, ##__VA_ARGS__)

#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define ETHER_TYPE_IPv6 0x86dd

int quit = 0;
int stats = 0;

struct pcapDeviceInfo {
    const char *name; // Interface to use to inject packets
    const char *mac; // Interface to copy MAC address from
    pcap_t     *descr;
    int         fd;
    enum { eL2DoneTouch, eL2Strip, eL2Prepend } l2handling;
};

struct ethernetHeader {
    unsigned char dst[6], src[6];
    __u16         ether_type;
} eHeader;

void signal_handler(int n)
{
    switch (n) {
    case SIGINT:
    case SIGTERM:
        quit = 1;
        break;
    case SIGUSR1:
        stats = 1;
        break;
    }
}

pcap_t* createDesc(const char * const iface, int snaplen, int promisc)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr = pcap_open_live(iface, snaplen, promisc, -1, errbuf);

    if(descr == NULL) {
        LogCrit("pcap_open_live(): %s", errbuf);
        exit(EXIT_FAILURE);
    }

    if(PCAP_ERROR == pcap_setnonblock(descr, 1, errbuf)) {
        LogCrit("pcap_setnonblock(): %s", errbuf);
        exit(EXIT_FAILURE);
    }

    LogInfo("Attached to network interface %s, snaplen %d", iface, snaplen);

    return descr;
}

//#define DO_LOG_MAC
#ifdef DO_LOG_MAC
const char * const mac2ascii(const unsigned char *mac)
{
    static char ascii[24]; // Need only 18 bytes, including terminating zero

    char *p = ascii;

    for (int i=0; i<6; ++i) {
        *p++ = "0123456789abcdef"[mac[i]>>4];
        *p++ = "0123456789abcdef"[mac[i]&0xf];
        *p++ = ':';
    }

    p[-1] = 0;

    return ascii;
}
#endif

void getMac(const char * const iface_name, unsigned char *mac, const char * const iface_mac)
{
    struct ifreq ifr;

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (0 > fd) {
        LogCrit("socket(): error %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    strncpy(ifr.ifr_name, iface_mac, sizeof(ifr.ifr_name)-1);
    ifr.ifr_name[sizeof(ifr.ifr_name)-1]='\0';

    if (0 > ioctl(fd, SIOCGIFHWADDR, &ifr)) {
        LogCrit("ioctl(): error %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(fd);

    for (int i=0; i<6; ++i) {
        mac[i] = ifr.ifr_hwaddr.sa_data[i];
    }

#ifdef DO_LOG_MAC
    LogInfo("Interface %s: using MAC %s", iface_name, mac2ascii(mac));
#endif
}

void showStats(pcap_t *p, const char *iface)
{
    struct pcap_stat ps;

    if (0 == pcap_stats(p, &ps)) {
        LogDebug("%s: recv %d, drop %d, ifdrop %d", iface, ps.ps_recv, ps.ps_drop, ps.ps_ifdrop);
    }
}

void callback(u_char *user, const struct pcap_pkthdr *header, const u_char *data)
{
    struct ethernetHeader {
        unsigned char dst[6], src[6];
        __u16         ether_type;
    };

    static u_char buf[8192];
    static int    have_eheader = 0;

    struct pcapDeviceInfo *iface = (struct pcapDeviceInfo *) user;

    if (header->caplen != header->len) {
        LogCrit("Received %d bytes, captured only %d", header->len, header->caplen);
        return;
    }

    switch (iface->l2handling) {
    case eL2DoneTouch: // Just pass L2 data along - used in pass-through mode
        if (0 != pcap_sendpacket(iface->descr, data, header->len)) {
            LogCrit("pcap_sendpacket(): %s", pcap_geterr(iface->descr));
        }
        break;

    case eL2Strip: // Strip L2 data when sending packets to enf0
        if (ntohs(((struct ethernetHeader*)data)->ether_type) == ETHER_TYPE_IPv6) {
            // An IPv6 Ethernet frame

            // Send packet, but strip Ethernet header (which is 14 bytes)
            if (0 != pcap_sendpacket(iface->descr,
                                     data+14,
                                     header->len-14)) {
                LogCrit("pcap_sendpacket(): %s", pcap_geterr(iface->descr));
            }
        }
        break;

    case eL2Prepend: // Add (prepend) L2 header data to packets to host
        if (0 == have_eheader) {
            // Copy prepared Ethernet header to buf just once
            memcpy(buf, &eHeader, sizeof(eHeader));
            have_eheader = 1;
        }

        // Copy packet data after local Ethernet header
        memcpy(buf+sizeof(struct ethernetHeader),data,header->len);

        // Then send IPv6 packet data
        if (0 != pcap_sendpacket(iface->descr,
                                 buf,
                                 sizeof(struct ethernetHeader)+header->len)) {
            LogCrit("pcap_sendpacket(): %s", pcap_geterr(iface->descr));
        }
        break;
    }
}

int main(int argc, char **argv)
{
    struct pcapDeviceInfo host_if = { .name = "usb0", .mac = "usb0", .l2handling = eL2DoneTouch };
    struct pcapDeviceInfo net_if = { .name = "wlan0", .mac = "wlan0", .l2handling = eL2DoneTouch };

    int snaplen = 8192;
    int opt;

    while(-1 != (opt = getopt(argc, argv, "2h:n:s:"))) {
        switch(opt) {
        case '2':
            net_if.l2handling = eL2Strip; // Strip L2 data from each packet
            host_if.l2handling = eL2Prepend; // Add L2 data to each packet
            break;
        case 'h':
            host_if.name = optarg;
            break;
        case 's':
            snaplen = atoi(optarg);
            break;
        case 'n':
            net_if.name = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-2] [-h host_if] [-n net_if] [-s snaplen]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    openlog("xbridge", LOG_PID, LOG_LOCAL0);
    LogInfo("Starting up");

    host_if.descr = createDesc(host_if.name, snaplen, 1);
    host_if.fd = pcap_get_selectable_fd(host_if.descr);

    net_if.descr = createDesc(net_if.name, snaplen, 0);
    net_if.fd = pcap_get_selectable_fd(net_if.descr);

    if (eL2DoneTouch != net_if.l2handling) {
        getMac(host_if.name, eHeader.src, host_if.mac);
        getMac(net_if.name, eHeader.dst, net_if.mac);
        eHeader.ether_type = htons(ETHER_TYPE_IPv6);
    }

    int maxfd = MAX(host_if.fd,net_if.fd);

    fd_set r_set;

    signal(SIGTERM, signal_handler); // kill -15
    signal(SIGINT, signal_handler); // Ctrl-C
    signal(SIGUSR1, signal_handler); // usr1, log stats

    while (!quit) {
        FD_ZERO(&r_set);
        FD_SET(host_if.fd, &r_set);
        FD_SET(net_if.fd, &r_set);

        int iret = select(maxfd + 1, &r_set, NULL, NULL, NULL);

        if (iret < 0 && EINTR != errno) {
            LogDebug("select() failed: %d (%s)", errno, strerror(errno));
        }

        if (iret > 0) {
            if (FD_ISSET(host_if.fd, &r_set)) {
                pcap_dispatch(host_if.descr, 9, callback, (u_char*) &net_if);
            }

            if (FD_ISSET(net_if.fd, &r_set)) {
                pcap_dispatch(net_if.descr, 9, callback, (u_char*) &host_if);
            }
        }

        if (stats) {
            showStats(host_if.descr, host_if.name);
            showStats(net_if.descr, net_if.name);
            stats = 0;
        }
    }

    LogInfo("Terminating...");

    showStats(host_if.descr, host_if.name);
    showStats(net_if.descr, net_if.name);

    pcap_close(host_if.descr);
    pcap_close(net_if.descr);

    LogInfo("Terminated");

    closelog();

    exit(EXIT_SUCCESS);
}
