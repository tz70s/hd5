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