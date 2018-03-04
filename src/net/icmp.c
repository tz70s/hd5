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

#include "icmp.h"
#include "config.h"
#include "ip.h"
#include "libc.h"
#include "utils.h"

int handle_icmp(uint8_t *buf) {
    struct icmpv4_packet *p = (struct icmpv4_packet *)buf;

    if (p->icmpv4_header.type != 0x08) return 1; /* not an echo request */

    if (p->icmpv4_header.code != 0x00) return 1;

    /* reorder ether net header addresses */
    memcpy(p->ether_header.target, p->ether_header.source, HLEN_ETHER);
    memcpy(p->ether_header.source, macaddr, HLEN_ETHER);

    p->ipv4_header.id = 0;
    p->ipv4_header.flags_offset = 0;

    /* reorder ipv4_header net header addresses */
    memcpy(p->ipv4_header.dst_ip, p->ipv4_header.src_ip, PLEN_IPV4);
    memcpy(p->ipv4_header.src_ip, ipaddr, PLEN_IPV4);

    /* recalculate ipv4_header checksum for return pkt */
    p->ipv4_header.checksum = 0;
    p->ipv4_header.checksum =
        checksum((uint16_t *)&p->ipv4_header, sizeof(struct ipv4_header));

    p->icmpv4_header.type = 0x0; /* change into reply */

    /* recalculate ICMP checksum */
    p->icmpv4_header.checksum = 0;
    p->icmpv4_header.checksum =
        checksum((uint16_t *)&p->icmpv4_header,
                 htons(p->ipv4_header.length) - sizeof(struct ipv4_header));
    return 0;
}