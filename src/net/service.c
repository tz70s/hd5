/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#include "service.h"
#include "arp.h"
#include "config.h"
#include "icmp.h"
#include "ip.h"
#include "libc.h"
#include "solo5.h"
#include "tcp.h"
#include "udp.h"
#include "utils.h"

#define PROTO_ICMP 0x01
#define PROTO_UDP 0x11
#define PROTO_TCP 0x06

/* FIXME: Remove evil global creation. */
uint8_t ipaddr[4] = {0x0a, 0x00, 0x00, 0x02};        /* 10.0.0.2 */
uint8_t ipaddr_brdnet[4] = {0x0a, 0x00, 0x00, 0xff}; /* 10.0.0.255 */
uint8_t ipaddr_brdall[4] = {0xff, 0xff, 0xff, 0xff}; /* 255.255.255.255 */
uint8_t macaddr[HLEN_ETHER];
uint8_t macaddr_brd[HLEN_ETHER] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const solo5_time_t NSEC_PER_SEC = 1000000000ULL;
bool is_send = false;

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

static int handle_ip(uint8_t *buf, size_t *len, enum tcp_state *state);

/*
 * FIXME: Currently, the internal occurred an race condition.
 * If we disable all the puts in wheter here or tcp.c.
 * Also, if the traffic comes from external(under nat transform).
 * The close state can't be reached.
 */
void net_serve() {
    struct solo5_net_info ni;
    solo5_net_info(&ni);
    memcpy(macaddr, ni.mac_address, sizeof macaddr);

    char macaddr_s[(sizeof macaddr * 2) + 1];
    tohexs(macaddr_s, macaddr, sizeof macaddr);
    puts("Serving ping on 10.0.0.2\n");

    send_garp();

    uint8_t buf[ni.mtu + SOLO5_NET_HLEN];
    enum tcp_state state = LISTEN;

    for (;;) {
        struct ether_header *p = (struct ether_header *)&buf;
        size_t len = 0;

        /* wait for packet */
        while (solo5_net_read(buf, sizeof buf, &len) == SOLO5_R_AGAIN) {
            if (state == LAST_ACK) return;
            solo5_yield(solo5_clock_monotonic() + NSEC_PER_SEC);
        }

        if (memcmp(p->target, macaddr, HLEN_ETHER) &&
            memcmp(p->target, macaddr_brd, HLEN_ETHER))
            continue; /* not ether addressed to us */

        int result_state = 0;
        switch (htons(p->type)) {
            case ETHERTYPE_ARP:
                if (handle_arp(buf) != 0) goto out;
                break;
            case ETHERTYPE_IP:
                result_state = handle_ip(buf, &len, &state);
                if (result_state != 0) {
                    if (result_state == -2) return;
                    if (state == CLOSE_WAIT) {
                        /* TODO: debug guard. */
                        /* Write out ack */
                        if (solo5_net_write(buf, len) != SOLO5_R_OK)
                            puts("Write error\n");
                        /* Write out fin */
                        handle_ip(buf, &len, &state);
                        puts("Write out fin\n");
                        /* break switch */
                        break;
                    }
                }
                if ((state == ESTABLISHED) && !is_send) {
                    /* TODO: debug guard. */
                    /* Write out previous msg. */
                    if (solo5_net_write(buf, len) != SOLO5_R_OK)
                        puts("Write error\n");
                    /* handling twice, send back msg */
                    state = SEND_MSG;
                    if (handle_ip(buf, &len, &state) != 0) goto out;
                }
                break;
            default:
                goto out;
        }

        /* TODO: debug guard. */
        if (solo5_net_write(buf, len) != SOLO5_R_OK) puts("Write error\n");

        /* TODO: not really need to clean up */
        memset(buf, 0, ni.mtu + SOLO5_NET_HLEN);

        continue;

    out:
        /* Ignore dropped. */
        UNUSED(1);
    }
}

static int handle_ip(uint8_t *buf, size_t *len, enum tcp_state *tcp_state) {
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
            result = handle_icmp(buf);
            break;
        case PROTO_UDP:
            result = handle_udp(buf);
            if (result == -1) return 1;
            if (result == -2) return -2;
            *len = result;
            result = 0;
            break;
        case PROTO_TCP:
            result = handle_tcp(buf, tcp_state);
            if (result == -1) return 1;
            *len = result;
            result = 0;
            break;
        default:
            return 1;
    }

    return result;
}