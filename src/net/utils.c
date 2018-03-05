/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
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
