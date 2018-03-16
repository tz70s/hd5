/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#include "tcp.h"
#include "config.h"
#include "http.h"
#include "libc.h"
#include "utils.h"

#define PROTO_TCP 0x06
#define SELF_TCP_PORT 1234

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

static uint16_t tcp4_checksum(struct ipv4_header *ipv4_header, const void *data,
                              size_t data_len);
static size_t handle_tcp_listen(struct tcp_packet *tcp_packet);
static size_t handle_tcp_send_msg(struct tcp_packet *tcp_packet);
static size_t handle_tcp_fin(struct tcp_packet *tcp_packet);
static size_t handle_tcp_established(struct tcp_packet *tcp_packet);

int handle_tcp(uint8_t *buf, enum tcp_state *tcp_state) {
    struct tcp_packet *p = (struct tcp_packet *)buf;
    if (p->tcp_header.dst_port != htons(SELF_TCP_PORT)) return -1;

    /* reorder ether net header addresses */
    memcpy(p->ether_header.target, p->ether_header.source, HLEN_ETHER);
    memcpy(p->ether_header.source, macaddr, HLEN_ETHER);

    p->ipv4_header.flags_offset = htons(1 << 14);

    /* reorder ipv4_header net header addresses */
    memcpy(p->ipv4_header.dst_ip, p->ipv4_header.src_ip, PLEN_IPV4);
    memcpy(p->ipv4_header.src_ip, ipaddr, PLEN_IPV4);

    /* exchange port */
    uint16_t tmp_port = p->tcp_header.src_port;
    p->tcp_header.src_port = p->tcp_header.dst_port;
    p->tcp_header.dst_port = tmp_port;

    size_t data_len = 0;
    switch (*tcp_state) {
        case LISTEN:
            data_len = handle_tcp_listen(p);
            /* Bump to syn received state. */
            *tcp_state = SYN_RECEIVED;
            break;
        case SYN_RECEIVED:
            /* Ignore acked msg */
            /* Bump to established state. */
            *tcp_state = ESTABLISHED;
            return -1;
            break;
        case ESTABLISHED:
            puts("Bump to established.\n");
            /* Receiving, playing back acked */
            if (handle_tcp_established(p) == 1) {
                /* accept fin, bump to close wait */
                *tcp_state = CLOSE_WAIT;
            }
            break;
        case SEND_MSG:
            puts("Bump to send msg.\n");
            data_len = handle_tcp_send_msg(p);
            /* Bump to syn received state, to dropped ack. */
            *tcp_state = ESTABLISHED;
            is_send = true;
            break;
        case CLOSE_WAIT:
            puts("Bump to close wait.\n");
            data_len = handle_tcp_fin(p);
            *tcp_state = LAST_ACK;
            break;
        case LAST_ACK:
            puts("Bump to last ack.\n");
            return -2;
        default:
            return -1;
    }

    /* recalculate ip header length */
    p->ipv4_header.length =
        htons(sizeof(struct ipv4_header) + TCP_HDR_LEN + data_len);

    /* recalculate ipv4_header checksum for return pkt */
    p->ipv4_header.checksum = 0;
    p->ipv4_header.checksum =
        checksum((uint16_t *)&p->ipv4_header, sizeof(struct ipv4_header));

    /* hard coded window size */
    p->tcp_header.win = htons(65535);

    /* calculate tcp4 checksum */
    p->tcp_header.checksum = 0;
    p->tcp_header.checksum = tcp4_checksum(
        &(p->ipv4_header),
        htod(buf, sizeof(struct ether_header) + sizeof(struct ipv4_header)),
        TCP_HDR_LEN + data_len);

    return sizeof(p->ether_header) + sizeof(struct ipv4_header) + TCP_HDR_LEN +
           data_len;
}

static size_t handle_tcp_listen(struct tcp_packet *tcp_packet) {
    struct tcp_header *tcp_header = &(tcp_packet->tcp_header);
    /* set flags */
    uint8_t off = TCP_HDR_LEN / 4;
    uint8_t resrv = 0;
    /* ACK + SYN for established connection. */
    uint8_t flag = 0b010010;

    tcp_header->off_flags =
        htons((uint16_t)((off << 12) + (resrv << 6) + flag));

    /* sequence number */
    uint32_t src_seq = tcp_header->seq_num;
    tcp_header->seq_num = (uint32_t)hton32(300);

    /* acked number */
    tcp_header->ack_num = hton32(hton32(src_seq) + 1);

    /* no payload for syn+ack message */
    return 0;
}

static size_t handle_tcp_send_msg(struct tcp_packet *tcp_packet) {
    struct tcp_header *tcp_header = &(tcp_packet->tcp_header);

    /* set flags */
    uint8_t off = TCP_HDR_LEN / 4;
    uint8_t resrv = 0;
    /* ACK only for playing back msg. */
    uint8_t flag = 0b010000;

    tcp_header->off_flags =
        htons((uint16_t)((off << 12) + (resrv << 6) + flag));

    /* sequence number */
    uint32_t src_seq = tcp_header->seq_num;
    uint32_t src_ack = tcp_header->ack_num;
    tcp_header->seq_num = src_ack;

    /* acked number */
    tcp_header->ack_num = hton32(hton32(src_seq) + 1);

    const char *msg = http_ok_response;
    size_t data_len = strlen(msg);
    memset(tcp_packet->data, 0, TCP_DATA_LEN);
    memcpy(tcp_packet->data, msg, data_len);
    return data_len;
}

static size_t handle_tcp_established(struct tcp_packet *tcp_packet) {
    struct tcp_header *tcp_header = &(tcp_packet->tcp_header);

    size_t result = 0;
    /* not fin */
    if (!(htons(tcp_header->off_flags) & 0x1)) {
        if (strlen((char *)tcp_packet->data) > 0) {
            puts("Received tcp msg : ");
            puts((char *)tcp_packet->data);
            puts("\n");
        }
    } else {
        puts("Accept fin\n");
        result = 1;
    }

    /* set flags */
    uint8_t off = TCP_HDR_LEN / 4;
    uint8_t resrv = 0;
    /* ACK only for playing back msg. */
    uint8_t flag = 0b010000;

    tcp_header->off_flags =
        htons((uint16_t)((off << 12) + (resrv << 6) + flag));

    /* sequence number */
    uint32_t src_seq = tcp_header->seq_num;
    uint32_t src_ack = tcp_header->ack_num;
    tcp_header->seq_num = src_ack;

    /* acked number */
    tcp_header->ack_num = hton32(hton32(src_seq) + 1);

    return result;
}

static size_t handle_tcp_fin(struct tcp_packet *tcp_packet) {
    struct tcp_header *tcp_header = &(tcp_packet->tcp_header);

    /* set flags */
    uint8_t off = TCP_HDR_LEN / 4;
    uint8_t resrv = 0;
    /* FIN msg. */
    uint8_t flag = 0b000001;

    tcp_header->off_flags =
        htons((uint16_t)((off << 12) + (resrv << 6) + flag));

    /* sequence number */
    uint32_t src_ack = tcp_header->ack_num;
    tcp_header->seq_num = src_ack;

    /* acked number */
    tcp_header->ack_num = 0;

    return 0;
}

static uint16_t tcp4_checksum(struct ipv4_header *ipv4_header, const void *data,
                              size_t data_len) {
    const uint16_t *buf = data;
    uint32_t checksum = 0;
    size_t checksum_len = data_len;
    uint16_t *src_ip = (void *)ipv4_header->src_ip;
    uint16_t *dst_ip = (void *)ipv4_header->dst_ip;

    while (checksum_len > 1) {
        checksum += *buf++;
        if (checksum & 0x80000000)
            checksum = (checksum & 0xffff) + (checksum >> 16);
        checksum_len -= 2;
    }

    /* add the padding if the packet length is odd */
    if (checksum_len > 0) checksum += *((uint8_t *)buf);

    /* add pseudo header */
    checksum += *(src_ip);
    checksum += *(++src_ip);
    checksum += *(dst_ip);
    checksum += *(++dst_ip);
    /* zero */
    checksum += htons(PROTO_TCP);
    checksum += htons(data_len);

    /* Fold 32-bit checksum to 16 bits */
    while (checksum >> 16) checksum = (checksum & 0xffff) + (checksum >> 16);

    return (uint16_t)(~checksum);
}