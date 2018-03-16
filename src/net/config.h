/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#ifndef HD5_NET_CONFIG_H_
#define HD5_NET_CONFIG_H_

#include "ether.h"

uint8_t ipaddr[4];
uint8_t ipaddr_brdnet[4];
uint8_t ipaddr_brdall[4];
uint8_t macaddr[HLEN_ETHER];
uint8_t macaddr_brd[HLEN_ETHER];

bool is_send;

#endif  // HD5_NET_CONFIG_H_