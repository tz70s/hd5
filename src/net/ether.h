/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#ifndef HD5_NET_ETHER_H_
#define HD5_NET_ETHER_H_

#include "solo5.h"

#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
#define HLEN_ETHER 6

struct ether_header {
    uint8_t target[HLEN_ETHER];
    uint8_t source[HLEN_ETHER];
    uint16_t type;
} __attribute__((packed, aligned(1)));

#endif  // HD5_NET_ETHER_H_