/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#ifndef HD5_NET_ICMP_H_
#define HD5_NET_ICMP_H_

#include "ether.h"
#include "ip.h"

struct icmpv4_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seqnum;
    uint8_t data[0];
} __attribute__((packed, aligned(1)));

struct icmpv4_packet {
    struct ether_header ether_header;
    struct ipv4_header ipv4_header;
    struct icmpv4_header icmpv4_header;
} __attribute__((packed, aligned(1)));

int handle_icmp(uint8_t *buf);

#endif  // HD5_NET_ICMP_H_