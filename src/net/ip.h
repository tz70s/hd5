/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#ifndef HD5_NET_IP_H_
#define HD5_NET_IP_H_

#include "solo5.h"

#define PLEN_IPV4 4

struct ipv4_header {
    uint8_t version_ihl;
    uint8_t type;
    uint16_t length;
    uint16_t id;
    uint16_t flags_offset;
    uint8_t ttjl;
    uint8_t proto;
    uint16_t checksum;
    uint8_t src_ip[PLEN_IPV4];
    uint8_t dst_ip[PLEN_IPV4];
} __attribute__((packed, aligned(1)));

#endif  // HD5_NET_IP_H_