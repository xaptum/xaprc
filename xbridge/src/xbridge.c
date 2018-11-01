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

#define LogDebug(format, ...) syslog(LOG_DEBUG, format, ##__VA_ARGS__)
#define LogCrit(format, ...) syslog(LOG_CRIT, format, ##__VA_ARGS__)
#define LogInfo(format, ...) syslog(LOG_INFO, format, ##__VA_ARGS__)

#define MAX(a,b) ((a) > (b) ? (a) : (b))

int quit = 0;
int stats = 0;

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

void showStats(pcap_t *p, const char *iface)
{
    struct pcap_stat ps;

    if (0 == pcap_stats(p, &ps)) {
        LogDebug("%s: recv %d, drop %d, ifdrop %d", iface, ps.ps_recv, ps.ps_drop, ps.ps_ifdrop);
    }
}

void callback(u_char *user, const struct pcap_pkthdr *header, const u_char *data)
{
    if (header->caplen == header->len) {
        if (0 != pcap_sendpacket((pcap_t*) user, data, header->len)) {
            LogCrit("pcap_sendpacket(): %s", pcap_geterr((pcap_t*) user));
        }
    } else
        LogCrit("Received %d bytes, captured only %d", header->len, header->caplen);
}

int main(int argc, char **argv)
{
    const char * host_if = "usb0";
    const char * wireless_if = "wlan0";
    int snaplen = 8192;
    int opt;

    while(-1 != (opt = getopt(argc, argv, "h:w:s:"))) {
        switch(opt) {
        case 'h':
            host_if = optarg;
            break;
        case 's':
            snaplen = atoi(optarg);
            break;
        case 'w':
            wireless_if = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-h host_if] [-w wireless_if] [-s snaplen]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    openlog("xbridge", LOG_PID, LOG_LOCAL0);
    LogInfo("Starting up");

    pcap_t *descr1 = createDesc(host_if, snaplen, 1);
    int fd1 = pcap_get_selectable_fd(descr1);

    pcap_t *descr2 = createDesc(wireless_if, snaplen, 0);
    int fd2 = pcap_get_selectable_fd(descr2);

    int maxfd = MAX(fd1,fd2);

    fd_set r_set;

    signal(SIGTERM, signal_handler); // kill -15
    signal(SIGINT, signal_handler); // Ctrl-C
    signal(SIGUSR1, signal_handler); // usr1, log stats

    while (!quit) {
        FD_ZERO(&r_set);
        FD_SET(fd1, &r_set);
        FD_SET(fd2, &r_set);

        int iret = select(maxfd + 1, &r_set, NULL, NULL, NULL);

        if (iret < 0 && EINTR != errno) {
            LogDebug("select() failed: %d (%s)", errno, strerror(errno));
        }

        if (iret > 0) {
            if (FD_ISSET(fd1, &r_set)) {
                pcap_dispatch(descr1, 9, callback, (u_char*)descr2);
            }

            if (FD_ISSET(fd2, &r_set)) {
                pcap_dispatch(descr2, 9, callback, (u_char*)descr1);
            }
        }

        if (stats) {
            showStats(descr1, host_if);
            showStats(descr2, wireless_if);
            stats = 0;
        }
    }

    LogInfo("Terminating...");

    showStats(descr1, host_if);
    showStats(descr2, wireless_if);

    pcap_close(descr1);
    pcap_close(descr2);

    LogInfo("Terminated");

    closelog();

    exit(EXIT_SUCCESS);
}
