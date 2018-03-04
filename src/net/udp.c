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

#include "udp.h"
#include "config.h"
#include "libc.h"
#include "utils.h"

#define PROTO_UDP 0x11
#define SELF_UDP_PORT 1234
#define DST_UDP_PORT 5678

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

static uint16_t udp4_checksum(struct ipv4_header *ipv4_header, const void *data,
                              size_t data_len);

/*
 * FIXME: The destination can't receive packet currently.
 */
int handle_udp(uint8_t *buf) {
    struct udp_packet *p = (struct udp_packet *)buf;
    if (p->udp_header.dst_port != htons(SELF_UDP_PORT)) return -1;

    puts("Receive udp packet, payload : ");
    char *content = (char *)p->data;
    puts(content);
    puts("\n");

    const char *msg = "Hello udp client!";
    size_t data_len = strlen(msg);

    /* reorder ether net header addresses */
    memcpy(p->ether_header.target, p->ether_header.source, HLEN_ETHER);
    memcpy(p->ether_header.source, macaddr, HLEN_ETHER);

    p->ipv4_header.flags_offset = htons(1 << 14);

    /* reorder ipv4_header net header addresses */
    memcpy(p->ipv4_header.dst_ip, p->ipv4_header.src_ip, PLEN_IPV4);
    memcpy(p->ipv4_header.src_ip, ipaddr, PLEN_IPV4);
    p->ipv4_header.length =
        htons(sizeof(struct ipv4_header) + UDP_HDR_LEN + data_len);

    /* recalculate ipv4_header checksum for return pkt */
    p->ipv4_header.checksum = 0;
    p->ipv4_header.checksum =
        checksum((uint16_t *)&p->ipv4_header, sizeof(struct ipv4_header));

    /* Handling udp fields */

    /* exchange port */
    p->udp_header.src_port = p->udp_header.dst_port;
    p->udp_header.dst_port = htons(DST_UDP_PORT);

    p->udp_header.length = htons(UDP_HDR_LEN + data_len);

    /* calculate udp4 checksum */
    p->udp_header.checksum =
        udp4_checksum(&(p->ipv4_header),
                      (void *)htod(buf, sizeof(struct ether_header) +
                                            sizeof(struct ipv4_header)),
                      p->udp_header.length);

    memset(p->data, 0, UDP_DATA_LEN);
    memcpy(p->data, msg, data_len);

    return sizeof(p->ether_header) + sizeof(struct ipv4_header) + UDP_HDR_LEN +
           data_len;
}

static uint16_t udp4_checksum(struct ipv4_header *ipv4_header, const void *data,
                              size_t data_len) {
    const uint16_t *buf = data;
    uint32_t checksum = 0;
    size_t checksum_len = data_len;
    uint16_t *src_ip = (void *)(ipv4_header->src_ip);
    uint16_t *dst_ip = (void *)(ipv4_header->dst_ip);

    while (checksum_len > 1) {
        checksum += *buf++;
        if (checksum & 0x80000000)
            checksum = (checksum & 0xFFFF) + (checksum >> 16);
        checksum_len -= 2;
    }

    /* add the padding if the packet length is odd */
    if (checksum_len & 1) checksum += *((uint8_t *)buf);

    /* add pseudo header */
    checksum += *(src_ip++);
    checksum += *(src_ip);
    checksum += *(dst_ip++);
    checksum += *(dst_ip);

    checksum += htons(PROTO_UDP);
    checksum += htons(checksum_len);

    /* add carries */

    /* Fold 32-bit checksum to 16 bits */
    while (checksum >> 16) checksum = (checksum & 0xffff) + (checksum >> 16);

    return (uint16_t)(~checksum);
}