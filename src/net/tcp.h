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

#ifndef HD5_NET_TCP_H_
#define HD5_NET_TCP_H_

#include "ether.h"
#include "ip.h"
#include "solo5.h"

#define TCP_DATA_LEN 1024
#define TCP_HDR_LEN 20

enum tcp_state {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT,
    SEND_MSG,
    PRE_CLOSE,
    RECV_ACK
};

struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t off_flags;
    uint16_t win;
    uint16_t checksum;
    uint16_t urg_ptr;
} __attribute__((packed, aligned(1)));

struct tcp_packet {
    struct ether_header ether_header;
    struct ipv4_header ipv4_header;
    struct tcp_header tcp_header;
    uint8_t data[TCP_DATA_LEN];
} __attribute__((packed, aligned(1)));

int handle_tcp(uint8_t *buf, enum tcp_state *tcp_state);

#endif