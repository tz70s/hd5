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

#include "arp.h"
#include "config.h"
#include "libc.h"
#include "solo5.h"
#include "utils.h"

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

int handle_arp(uint8_t *buf) {
    struct arp_packet *p = (struct arp_packet *)buf;

    if (p->arp_header.htype != htons(1)) return 1;

    if (p->arp_header.ptype != htons(ETHERTYPE_IP)) return 1;

    if (p->arp_header.hlen != HLEN_ETHER || p->arp_header.plen != PLEN_IPV4)
        return 1;

    if (p->arp_header.op != htons(1)) return 1;

    if (memcmp(p->arp_header.tpa, ipaddr, PLEN_IPV4)) return 1;

    /* reorder ether net header addresses */
    memcpy(p->ether_header.target, p->ether_header.source, HLEN_ETHER);
    memcpy(p->ether_header.source, macaddr, HLEN_ETHER);
    memcpy(p->arp_header.tha, p->arp_header.sha, HLEN_ETHER);
    memcpy(p->arp_header.sha, macaddr, HLEN_ETHER);

    /* request -> reply */
    p->arp_header.op = htons(2);

    /* spa -> tpa */
    memcpy(p->arp_header.tpa, p->arp_header.spa, PLEN_IPV4);

    /* our ip -> spa */
    memcpy(p->arp_header.spa, ipaddr, PLEN_IPV4);

    return 0;
}

void send_garp(void) {
    struct arp_packet p;
    uint8_t zero[HLEN_ETHER] = {0};

    /*
     * Send a gratuitous ARP packet announcing our MAC address.
     */
    memcpy(p.ether_header.source, macaddr, HLEN_ETHER);
    memcpy(p.ether_header.target, macaddr_brd, HLEN_ETHER);
    p.ether_header.type = htons(ETHERTYPE_ARP);
    p.arp_header.htype = htons(1);
    p.arp_header.ptype = htons(ETHERTYPE_IP);
    p.arp_header.hlen = HLEN_ETHER;
    p.arp_header.plen = PLEN_IPV4;
    p.arp_header.op = htons(1);
    memcpy(p.arp_header.sha, macaddr, HLEN_ETHER);
    memcpy(p.arp_header.tha, zero, HLEN_ETHER);
    memcpy(p.arp_header.spa, ipaddr, PLEN_IPV4);
    memcpy(p.arp_header.tpa, ipaddr, PLEN_IPV4);

    if (solo5_net_write((uint8_t *)&p, sizeof p) != SOLO5_R_OK)
        puts("Could not send GARP packet\n");
}
