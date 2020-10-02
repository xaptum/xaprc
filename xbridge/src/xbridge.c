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
#include <time.h>
#include <unistd.h>
#include <linux/types.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/icmp6.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <sys/ioctl.h>

#define LogDebug(format, ...) if(debug) syslog(LOG_DEBUG, format, ##__VA_ARGS__)
#define LogCrit(format, ...) syslog(LOG_CRIT, format, ##__VA_ARGS__)
#define LogInfo(format, ...) syslog(LOG_INFO, format, ##__VA_ARGS__)

#define MAX(a,b) ((a) > (b) ? (a) : (b))

const int NEIGHBOR_ADV_PERIOD = 600; // Send unsolicited NA period

const struct in6_addr linkLocalDefaultGatewayIPv6 = {
    .s6_addr = { // fe80::1
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
    }
};

struct pcapDeviceInfo {
    const char *name; // Interface to use to inject packets
    const char *mac; // Interface to copy MAC address from
    pcap_t     *descr;
    int         fd;
    enum { eL2DoneTouch, eL2Strip, eL2Prepend } l2handling;

    struct pcapDeviceInfo *other; // Pointer to other network interface
};

int quit = 0; // quit when set
int stats = 0; // dump statistics when set
int na = 0; // send NA when set
int debug = 1;

struct ether_header eHeader;

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
    case SIGUSR2:
        na = 1;
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

    LogDebug("Attached to network interface %s, snaplen %d", iface, snaplen);

    return descr;
}

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

    LogDebug("Interface %s: using MAC %s", iface_name, mac2ascii(mac));
}

void showStats(pcap_t *p, const char *iface)
{
    struct pcap_stat ps;

    if (0 == pcap_stats(p, &ps)) {
        LogInfo("%s: recv %d, drop %d, ifdrop %d", iface, ps.ps_recv, ps.ps_drop, ps.ps_ifdrop);
    }
}

void sendPacket(const struct pcapDeviceInfo *iface, int len, const u_char *data)
{
    static u_char buf[8192];
    static int    have_eheader = 0;

    switch (iface->l2handling) {
    case eL2DoneTouch: // Just pass L2 data along - used in pass-through mode
        if (0 != pcap_sendpacket(iface->descr, data, len)) {
            LogCrit("pcap_sendpacket(): %s", pcap_geterr(iface->descr));
        }
        break;

    case eL2Strip: // Strip L2 data when sending packets to enf0
        if (ntohs(((struct ether_header*)data)->ether_type) == ETHERTYPE_IPV6) {
            // An IPv6 Ethernet frame

            // Send packet, but strip Ethernet header (which is 14 bytes)
            if (0 != pcap_sendpacket(iface->descr,
                                     data+sizeof(struct ether_header),
                                     len-sizeof(struct ether_header))) {
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
        memcpy(buf+sizeof(struct ether_header),data,len);

        // Then send IPv6 packet data
        if (0 != pcap_sendpacket(iface->descr,
                                 buf,
                                 sizeof(struct ether_header)+len)) {
            LogCrit("pcap_sendpacket(): %s", pcap_geterr(iface->descr));
        }
        break;
    }
}

uint16_t checksum(const void *data, int len)
{
    uint32_t sum = 0;
    uint16_t *p = (uint16_t*) data;

    while(len>1) {
        sum += *p++;
        len -= 2;
    }

    if (len) {
        sum += *((uint8_t*)p);
    }

    while (sum>>16)
        sum = (sum>>16) + (sum&0xFFFF);

    return sum;
}

int isNeighborSolicitation(const u_char *data, int len)
{
    struct ns {
        struct ip6_hdr ip6;
        struct nd_neighbor_solicit icmp6;
    } *packet = (struct ns*) data;

    // Perform a series of cheap and easy checks first
    if (sizeof(struct ns) > len)                                   return 0;
    if (sizeof(struct nd_neighbor_solicit) > packet->ip6.ip6_plen) return 0;
    if (IPPROTO_ICMPV6 != packet->ip6.ip6_nxt)                     return 0;
    if (ND_NEIGHBOR_SOLICIT != packet->icmp6.nd_ns_type)           return 0;
    if (0 != packet->icmp6.nd_ns_code)                             return 0;

    // Slightly more expensive checks
    if (htonl(0x60000000) != (htonl(0xF0000000)&packet->ip6.ip6_flow)) return 0;
    if (0 != memcmp(&packet->icmp6.nd_ns_target,
                    &linkLocalDefaultGatewayIPv6,
                    sizeof(linkLocalDefaultGatewayIPv6)))              return 0;

    // Most expensive check: verify checksum

    // IPv6 pseudo header
    struct {
        struct in6_addr src;
        struct in6_addr dst;
        uint32_t payload;
        uint32_t next_header;
    } ph = {
        .src = packet->ip6.ip6_src,
        .dst = packet->ip6.ip6_dst,
        .payload = htons(len - sizeof(struct ip6_hdr)),
        .next_header = htonl(IPPROTO_ICMPV6),
    };

    uint16_t csum[2] = {
        checksum(&ph, sizeof(ph)),
        checksum(&packet->icmp6, len-sizeof(struct ip6_hdr))
    };

    if (0xffff != checksum(csum,sizeof(csum))) return 0;

    // All checks ok, this is a Neighbor Solicitation packet we're looking for
    return 1;
}

int sendNeighborAdvert(const struct pcapDeviceInfo *iface, const struct in6_addr *dst, int solicited)
{
    if (solicited)
        solicited = ND_NA_FLAG_SOLICITED;

    // IPv6 header         and ICMPv6 neighbor advert
    struct {
        struct ip6_hdr ip6;
        struct nd_neighbor_advert icmp6;
        struct nd_opt_hdr target_lladdr;
        struct ether_addr target_lladdr_mac;
    } packet = {
        .ip6.ip6_flow = htonl(0x60000000), // 6 (IPv6), flow = 0
        .ip6.ip6_plen = htons(sizeof(packet)-sizeof(packet.ip6)),
        .ip6.ip6_nxt = IPPROTO_ICMPV6,
        .ip6.ip6_hlim = 255,
        .ip6.ip6_src = linkLocalDefaultGatewayIPv6,
        .ip6.ip6_dst = *dst,

        .icmp6.nd_na_type = ND_NEIGHBOR_ADVERT,
        .icmp6.nd_na_code = 0,
        // .icmp6.nd_na_cksum = set below
        .icmp6.nd_na_flags_reserved = solicited | ND_NA_FLAG_ROUTER,
        .icmp6.nd_na_target = linkLocalDefaultGatewayIPv6,

        .target_lladdr.nd_opt_type = ND_OPT_TARGET_LINKADDR,
        .target_lladdr.nd_opt_len = 1, // 8 octets
        // .target_lladdr_mac = set below
    };

    // IPv6 pseudo header
    struct {
        struct in6_addr src;
        struct in6_addr dst;
        uint32_t payload;
        uint32_t next_header;
    } ph = {
        .src = linkLocalDefaultGatewayIPv6,
        .dst = *dst,
        .payload = htons(sizeof(packet)-sizeof(packet.ip6)),
        .next_header = htonl(IPPROTO_ICMPV6),
    };

    // Copy our MAC address into lladdr
    memcpy(&packet.target_lladdr_mac, eHeader.ether_shost, sizeof(packet.target_lladdr_mac));

    // Calculate checksum
    packet.icmp6.nd_na_cksum = checksum(&ph, sizeof(ph));
    packet.icmp6.nd_na_cksum = ~checksum(&packet.icmp6, sizeof(packet)-sizeof(packet.ip6));
    // TODO check case csum == 0x0000? But is only relevant for UDP?

    sendPacket(iface, sizeof(packet), (u_char *) &packet);

    LogDebug("Sent %s Neighbor Advertisement", solicited ? "Solicited" : "Unsolicited");

    return 0;
}

void callback(u_char *user, const struct pcap_pkthdr *header, const u_char *data)
{
    if (header->caplen != header->len) {
        LogCrit("Received %d bytes, captured only %d", header->len, header->caplen);
        return;
    }

    struct pcapDeviceInfo *iface = (struct pcapDeviceInfo *) user;

    if (eL2Strip == iface->l2handling && // <- this is only true for pkts rcvd from host
        isNeighborSolicitation(data+sizeof(struct ether_header),
                               header->len-sizeof(struct ether_header))) {
        // We received, from the host, a Neighbor Solicitation package that matches
        // with those we're looking for. Do not forward this to the enf0, but instead,
        // send a Neighbor Advertisement response directly to the host.

        // Get the src IPv6 address from the packet
        struct ip6_hdr *header = (struct ip6_hdr*) (data+sizeof(struct ether_header));

        // Note that 'iface' currently points to net_if (aka 'enf0'), but we
        // need to send the NA to the host_if
        sendNeighborAdvert(iface->other, &header->ip6_src, 1);
    } else {
        // Normal case pass packet from one interface to the other
        sendPacket(iface, header->len, data);
    }
}

int main(int argc, char **argv)
{
    struct pcapDeviceInfo host_if = { .name = "usb0", .mac = "usb0", .l2handling = eL2DoneTouch };
    struct pcapDeviceInfo net_if = { .name = "wlan0", .mac = "wlan0", .l2handling = eL2DoneTouch };

    int snaplen = 8192;
    int opt;
    int promiscuous = 0;

    while(-1 != (opt = getopt(argc, argv, "2h:n:s:p"))) {
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
        case 'p':
            promiscuous = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-2] [-h host_if] [-n net_if] [-s snaplen] [-p]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Keep a pointer to the 'other' interface within each iface struct; this allows
    // easy access to whatever interface we need given either struct, which is required
    // in case we need to respond to a Neighbor Solicitation
    host_if.other = &net_if;
    net_if.other = &host_if;

    openlog("xbridge", LOG_PID, LOG_LOCAL0);
    LogInfo("Starting up");

    host_if.descr = createDesc(host_if.name, snaplen, 1);
    host_if.fd = pcap_get_selectable_fd(host_if.descr);

    net_if.descr = createDesc(net_if.name, snaplen, promiscuous);
    net_if.fd = pcap_get_selectable_fd(net_if.descr);

    if (eL2DoneTouch != net_if.l2handling) {
        getMac(host_if.name, eHeader.ether_shost, host_if.mac);
        getMac(net_if.name, eHeader.ether_dhost, net_if.mac);
        eHeader.ether_type = htons(ETHERTYPE_IPV6);
    }

    int maxfd = MAX(host_if.fd,net_if.fd);

    fd_set r_set;

    signal(SIGTERM, signal_handler); // kill -15
    signal(SIGINT, signal_handler); // Ctrl-C
    signal(SIGUSR1, signal_handler); // usr1, log stats
    signal(SIGUSR2, signal_handler); // usr2, send (unsolicited) na

    time_t na_timeout = 0;

    while (!quit) {
        FD_ZERO(&r_set);
        FD_SET(host_if.fd, &r_set);
        FD_SET(net_if.fd, &r_set);

        time_t now = time(NULL);
        struct timeval tv = { .tv_sec = MAX(1, na_timeout-now), .tv_usec = 0 };
        int iret = select(maxfd + 1, &r_set, NULL, NULL, &tv);

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

            now = time(NULL);
            LogInfo("Next unsolicited NA in %ld secs", MAX(1,na_timeout-now));

            stats = 0;
        }

        if (na || (time(NULL)>na_timeout)) {
            // Send unsolicited NA

            const struct in6_addr all_nodes = {
                .s6_addr = { // ff02::1
                    0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
                }
            };

            sendNeighborAdvert(&host_if, &all_nodes, 0);
            na = 0;
            na_timeout = NEIGHBOR_ADV_PERIOD + time(NULL);
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
