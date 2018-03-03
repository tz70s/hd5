/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "service.h"
#include "arp.h"
#include "config.h"
#include "icmp.h"
#include "ip.h"
#include "libc.h"
#include "solo5.h"
#include "utils.h"

/*FIXME: Remove evil global creation. */
uint8_t ipaddr[4] = {0x0a, 0x00, 0x00, 0x02};        /* 10.0.0.2 */
uint8_t ipaddr_brdnet[4] = {0x0a, 0x00, 0x00, 0xff}; /* 10.0.0.255 */
uint8_t ipaddr_brdall[4] = {0xff, 0xff, 0xff, 0xff}; /* 255.255.255.255 */
uint8_t macaddr[HLEN_ETHER];
uint8_t macaddr_brd[HLEN_ETHER] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

void net_serve(int verbose, int limit) {
    unsigned long received = 0;

    struct solo5_net_info ni;
    solo5_net_info(&ni);
    memcpy(macaddr, ni.mac_address, sizeof macaddr);

    char macaddr_s[(sizeof macaddr * 2) + 1];
    tohexs(macaddr_s, macaddr, sizeof macaddr);
    puts("Serving ping on 10.0.0.2, with MAC: ");
    puts(macaddr_s);
    puts("\n");

    send_garp();

    uint8_t buf[ni.mtu + SOLO5_NET_HLEN];

    for (;;) {
        struct ether_header *p = (struct ether_header *)&buf;
        size_t len;

        /* wait for packet */
        while (solo5_net_read(buf, sizeof buf, &len) == SOLO5_R_AGAIN) {
            solo5_yield(solo5_clock_monotonic() + NSEC_PER_SEC);
        }

        if (memcmp(p->target, macaddr, HLEN_ETHER) &&
            memcmp(p->target, macaddr_brd, HLEN_ETHER))
            continue; /* not ether addressed to us */

        switch (htons(p->type)) {
            case ETHERTYPE_ARP:
                if (handle_arp(buf) != 0) goto out;
                if (verbose) puts("Received arp request, sending reply\n");
                break;
            case ETHERTYPE_IP:
                if (handle_ip(buf) != 0) goto out;
                if (verbose) puts("Received ping, sending reply\n");
                break;
            default:
                goto out;
        }

        if (solo5_net_write(buf, len) != SOLO5_R_OK) puts("Write error\n");

        received++;
        if (limit && received == 100000) {
            puts("Limit reached, exiting\n");
            break;
        }

        continue;

    out:
        puts("Received non-ping or unsupported packet, dropped\n");
    }
}
