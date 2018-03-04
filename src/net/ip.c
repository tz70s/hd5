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
#include "config.h"
#include "icmp.h"
#include "libc.h"
#include "solo5.h"
#include "udp.h"
#include "utils.h"

#define PROTO_ICMP 0x01
#define PROTO_UDP 0x11
#define PROTO_TCP 0x06

#define SELF_UDP_PORT 1234
#define DST_UDP_PORT 5678

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

static int handle_icmp(uint8_t *buf) {
    struct icmpv4_packet *p = (struct icmpv4_packet *)buf;

    if (p->icmpv4_header.type != 0x08) return 1; /* not an echo request */

    if (p->icmpv4_header.code != 0x00) return 1;

    /* reorder ether net header addresses */
    memcpy(p->ether_header.target, p->ether_header.source, HLEN_ETHER);
    memcpy(p->ether_header.source, macaddr, HLEN_ETHER);

    p->ipv4_header.id = 0;
    p->ipv4_header.flags_offset = 0;

    /* reorder ipv4_header net header addresses */
    memcpy(p->ipv4_header.dst_ip, p->ipv4_header.src_ip, PLEN_IPV4);
    memcpy(p->ipv4_header.src_ip, ipaddr, PLEN_IPV4);

    /* recalculate ipv4_header checksum for return pkt */
    p->ipv4_header.checksum = 0;
    p->ipv4_header.checksum =
        checksum((uint16_t *)&p->ipv4_header, sizeof(struct ipv4_header));

    p->icmpv4_header.type = 0x0; /* change into reply */

    /* recalculate ICMP checksum */
    p->icmpv4_header.checksum = 0;
    p->icmpv4_header.checksum =
        checksum((uint16_t *)&p->icmpv4_header,
                 htons(p->ipv4_header.length) - sizeof(struct ipv4_header));
    return 0;
}

/*
 * FIXME: The destination can't receive packet currently.
 */
static int handle_udp(uint8_t *buf) {
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

int handle_ip(uint8_t *buf, size_t *len) {
    int result = 0;

    /* move pointer to ip header. */
    struct ipv4_header *ipv4_header =
        (struct ipv4_header *)htod(buf, sizeof(struct ether_header));

    if (ipv4_header->version_ihl != 0x45)
        return 1; /* we don't support IPv6, yet :-) */

    if (ipv4_header->type != 0x00) return 1;

    /* check if address is point to self */
    if (memcmp(ipv4_header->dst_ip, ipaddr, PLEN_IPV4) &&
        memcmp(ipv4_header->dst_ip, ipaddr_brdnet, PLEN_IPV4) &&
        memcmp(ipv4_header->dst_ip, ipaddr_brdall, PLEN_IPV4))
        return 1; /* not ipv4_header addressed to us */

    switch (ipv4_header->proto) {
        case PROTO_ICMP:
            puts("Receive icmp packet! Sending reply.\n");
            result = handle_icmp(buf);
            break;
        case PROTO_UDP:
            result = handle_udp(buf);
            if (result == -1) return 1;
            *len = result;
            result = 0;
            break;
        default:
            return 1;
    }

    return result;
}