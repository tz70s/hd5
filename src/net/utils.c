/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#include "utils.h"

/* Copied from https://tools.ietf.org/html/rfc1071 */
uint16_t checksum(uint16_t *addr, size_t count) {
    /* Compute Internet Checksum for "count" bytes
     * beginning at location "addr".*/
    register long sum = 0;

    while (count > 1) {
        /*  This is the inner loop */
        sum += *(unsigned short *)addr++;
        count -= 2;
    }

    /* Add left-over byte, if any */
    if (count > 0) sum += *(unsigned char *)addr;

    /* Fold 32-bit sum to 16 bits */
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

void tohexs(char *dst, uint8_t *src, size_t size) {
    while (size--) {
        uint8_t n = *src >> 4;
        *dst++ = (n < 10) ? (n + '0') : (n - 10 + 'a');
        n = *src & 0xf;
        *dst++ = (n < 10) ? (n + '0') : (n - 10 + 'a');
        src++;
    }
    *dst = '\0';
}
