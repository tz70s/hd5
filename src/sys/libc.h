/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
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