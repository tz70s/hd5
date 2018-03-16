/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
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