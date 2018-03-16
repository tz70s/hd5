/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#ifndef HD5_NET_ARP_H_
#define HD5_NET_ARP_H_

#include "ether.h"
#include "ip.h"

struct arp_header {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t op;
    uint8_t sha[HLEN_ETHER];
    uint8_t spa[PLEN_IPV4];
    uint8_t tha[HLEN_ETHER];
    uint8_t tpa[PLEN_IPV4];
} __attribute__((packed, aligned(1)));

struct arp_packet {
    struct ether_header ether_header;
    struct arp_header arp_header;
} __attribute__((packed, aligned(1)));

int handle_arp(uint8_t *buf);

void send_garp(void);

#endif  // HD5_NET_ARP_H_