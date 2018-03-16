/*
 * Copyright (c) 2018 Tzu-Chiao Yeh
 *
 * The hd5 project is licensed under MIT License.
 * Checkout LICENSE file in the root of source tree.
 */

#ifndef HD5_NET_HTTP_H_
#define HD5_NET_HTTP_H_

static const char *http_ok_response =
    "HTTP/1.1 200 OK\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Content-Length: 13\r\n\
Connection: close\r\n\r\n\
Hello World!\n";

#endif  // HD5_NET_HTTP_H