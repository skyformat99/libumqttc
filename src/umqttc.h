/*
 * Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UMQTTC_H
#define _UMQTTC_H

#include <libubox/uloop.h>
#include <libubox/ustream.h>

#include "config.h"
#include "log.h"

#if (UMQTTC_SSL_SUPPORT)
#include <libubox/ustream-ssl.h>
#endif

#define UMQTTC_PING_INTERVAL  30

enum umqttc_error_code {
    UMQTTC_ERROR_WRITE,
    UMQTTC_ERROR_INVALID_HEADER,
    UMQTTC_ERROR_NOT_SUPPORT_FREGMENT,
    UMQTTC_ERROR_SSL,
    UMQTTC_ERROR_SSL_INVALID_CERT,
    UMQTTC_ERROR_SSL_CN_MISMATCH
};

enum client_state {
    CLIENT_STATE_INIT,
    CLIENT_STATE_HANDSHAKE,
    CLIENT_STATE_MESSAGE
};

struct umqttc_frame {
    uint8_t fin;
    uint8_t opcode;
    uint64_t payload_len;
    char *payload;
};

struct umqttc_client {
    struct ustream *us;
    struct ustream_fd sfd;
    enum client_state state;
    struct umqttc_frame frame;
    struct uloop_timeout timeout;
    enum umqttc_error_code error;
    
#if (UMQTTC_SSL_SUPPORT)
    bool ssl_require_validation;
    struct ustream_ssl ussl;
    struct ustream_ssl_ctx *ssl_ctx;
    const struct ustream_ssl_ops *ssl_ops;
#endif

    void (*free)(struct umqttc_client *cl);
};

struct umqttc_client *umqttc_new_ssl(const char *url, const char *ca_crt_file, bool verify);

static inline struct umqttc_client *umqttc_new(const char *url)
{
    return umqttc_new_ssl(url, NULL, false);
}

#endif
