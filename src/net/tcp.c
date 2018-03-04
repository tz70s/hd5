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

#include "tcp.h"
#include "config.h"
#include "libc.h"
#include "utils.h"

#define PROTO_TCP 0x06
#define SELF_TCP_PORT 1234

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

static uint16_t tcp4_checksum(struct ipv4_header *ipv4_header, const void *data,
                              size_t data_len);
static void handle_tcp_listen(struct tcp_header *tcp_header);
static void handle_tcp_established(struct tcp_header *tcp_header);

int handle_tcp(uint8_t *buf, enum tcp_state *tcp_state) {
    struct tcp_packet *p = (struct tcp_packet *)buf;
    UNUSED(puts);
    if (p->tcp_header.dst_port != htons(SELF_TCP_PORT)) return -1;

    const char *msg = "Hello tcp client!";
    size_t data_len = strlen(msg);

    /* reorder ether net header addresses */
    memcpy(p->ether_header.target, p->ether_header.source, HLEN_ETHER);
    memcpy(p->ether_header.source, macaddr, HLEN_ETHER);

    p->ipv4_header.flags_offset = htons(1 << 14);

    /* reorder ipv4_header net header addresses */
    memcpy(p->ipv4_header.dst_ip, p->ipv4_header.src_ip, PLEN_IPV4);
    memcpy(p->ipv4_header.src_ip, ipaddr, PLEN_IPV4);
    p->ipv4_header.length =
        htons(sizeof(struct ipv4_header) + TCP_HDR_LEN + data_len);

    /* recalculate ipv4_header checksum for return pkt */
    p->ipv4_header.checksum = 0;
    p->ipv4_header.checksum =
        checksum((uint16_t *)&p->ipv4_header, sizeof(struct ipv4_header));

    /* Handling tcp fields */

    /* exchange port */
    uint16_t tmp_port = p->tcp_header.src_port;
    p->tcp_header.src_port = p->tcp_header.dst_port;
    p->tcp_header.dst_port = tmp_port;

    switch (*tcp_state) {
        case LISTEN:
            handle_tcp_listen(&(p->tcp_header));
            break;
        case ESTABLISHED:
            handle_tcp_established(&(p->tcp_header));
            break;
        default:
            return -1;
    }

    /* hard coded window size */
    p->tcp_header.win = htons(65535);

    /* FIXME: The checksum is not correct (from tcpdump) */
    /* calculate tcp4 checksum */
    p->tcp_header.checksum = tcp4_checksum(
        &(p->ipv4_header),
        htod(buf, sizeof(struct ether_header) + sizeof(struct ipv4_header)),
        TCP_HDR_LEN + data_len);

    memset(p->data, 0, TCP_DATA_LEN);
    memcpy(p->data, msg, data_len);

    return sizeof(p->ether_header) + sizeof(struct ipv4_header) + TCP_HDR_LEN +
           data_len;
}

static void handle_tcp_listen(struct tcp_header *tcp_header) {
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
}

static void handle_tcp_established(struct tcp_header *tcp_header) {
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