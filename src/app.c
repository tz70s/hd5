/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * This file is redistributed from solo5.
 * See https://github.com/Solo5/solo5
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#include "libc.h"
#include "net/config.h"
#include "net/service.h"
#include "solo5.h"

static void puts(const char *s) { solo5_console_write(s, strlen(s)); }

int solo5_app_main(const struct solo5_start_info *si) {
    puts("\n**** Spawn an hd5 instance! ****\n\n");

    size_t len = 0;
    const char *p = si->cmdline;

    while (*p++) len++;
    solo5_console_write(si->cmdline, len);

    net_serve();

    return SOLO5_EXIT_SUCCESS;
}