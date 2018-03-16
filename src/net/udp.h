/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#include "ether.h"
#include "ip.h"

#ifndef HD5_NET_UDP_H_
#define HD5_NET_UDP_H_

#define UDP_HDR_LEN 8
#define UDP_DATA_LEN 1024

/*
 * User Datagram Header Format.
 *
 *  0      7 8     15 16    23 24    315.
 * +--------+--------+--------+--------+
 * |      Source     |    Destination  |
 * |       Port      |       Port      |
 * +--------+--------+--------+--------+
 * |      Length     |     Checksum    |
 * +--------+--------+--------+--------+
 * |
 * |        data octets ...
 * +--------------- ...
 */

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
};

struct udp_packet {
    struct ether_header ether_header;
    struct ipv4_header ipv4_header;
    struct udp_header udp_header;
    uint8_t data[UDP_DATA_LEN];
} __attribute__((packed, aligned(1)));

int handle_udp(uint8_t *buf);

#endif  // HD5_NET_UDP_H_