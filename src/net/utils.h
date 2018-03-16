/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#ifndef HD5_NET_UTILS_H_
#define HD5_NET_UTILS_H_

#include "solo5.h"

#define UNUSED(x) (void)(x)

uint16_t checksum(uint16_t *addr, size_t count);

inline uint16_t htons(uint16_t x) { return (x << 8) + (x >> 8); }

inline uint32_t hton32(uint32_t x) {
    return (x >> 24) + (x << 24) + (((x >> 16) & 0xff) << 8) +
           ((x & 0xff00) << 8);
}

inline void *htod(void *ptr, size_t offset) { return (void *)(ptr + offset); }

void tohexs(char *dst, uint8_t *src, size_t size);

#endif  // HD5_NET_UTILS_H_