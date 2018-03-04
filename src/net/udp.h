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

/*
 * UDP Pseudo Header.
 * https://tools.ietf.org/html/rfc768
 *
 * 0      7 8     15 16    23 24    31
 * +--------+--------+--------+--------+
 * |          source address           |
 * +--------+--------+--------+--------+
 * |        destination address        |
 * +--------+--------+--------+--------+
 * |  zero  |protocol|   UDP length    |
 * +--------+--------+--------+--------+
 */

struct udp_pseudo_header {
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint8_t zero;
    uint8_t protocol;
    uint16_t udp_length;
};

struct udp_packet {
    struct ether_header ether_header;
    struct ipv4_header ipv4_header;
    struct udp_header udp_header;
    uint8_t data[UDP_DATA_LEN];
};

uint16_t udp4_checksum(struct ipv4_header *ipv4_header, const void *data,
                       size_t data_len);

#endif  // HD5_NET_UDP_H_