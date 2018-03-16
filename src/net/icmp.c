/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
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