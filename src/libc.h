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

#ifndef HD5_LIBC_H_
#define HD5_LIBC_H_

#include "solo5.h"

/* Re-export libc in solo5 */

void *memset(void *dest, int c, size_t n);

void *memcpy(void *restrict dest, const void *restrict src, size_t n);

void *memmove(void *dest, const void *src, size_t n);

int memcmp(const void *vl, const void *vr, size_t n);

int strcmp(const char *l, const char *r);

char *strcpy(char *restrict dest, const char *restrict src);

size_t strlen(const char *s);

int isspace(int c);

int strncmp(const char *_l, const char *_r, size_t n);

#endif  // HD5_LIBC_H_