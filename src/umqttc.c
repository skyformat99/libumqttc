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

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <glob.h>
#include <arpa/inet.h>
#include <libubox/usock.h>
#include <libubox/utils.h>

#include "umqttc.h"
#include "log.h"
#include "utils.h"

static void umqttc_free(struct umqttc_client *cl)
{
    uloop_timeout_cancel(&cl->timeout);
    ustream_free(&cl->sfd.stream);
    shutdown(cl->sfd.fd.fd, SHUT_RDWR);
    close(cl->sfd.fd.fd);
#if (UWSC_SSL_SUPPORT)
    if (cl->ssl_ops && cl->ssl_ctx)
        cl->ssl_ops->context_free(cl->ssl_ctx);
#endif
    free(cl);
}

static inline void umqttc_error(struct umqttc_client *cl, int error)
{
    cl->us->eof = true;
    cl->error = error;
    ustream_state_change(cl->us);
}

static inline void umqttc_notify_read(struct ustream *s, int bytes)
{
}

static inline void umqttc_notify_state(struct ustream *s)
{
}

#if (UWSC_SSL_SUPPORT)
static inline void umqttc_ssl_notify_read(struct ustream *s, int bytes)
{
}

static inline void umqttc_ssl_notify_state(struct ustream *s)
{
}

static void umqttc_ssl_notify_error(struct ustream_ssl *ssl, int error, const char *str)
{
}

static void umqttc_ssl_notify_verify_error(struct ustream_ssl *ssl, int error, const char *str)
{
}

static void umqttc_ssl_notify_connected(struct ustream_ssl *ssl)
{
}

#endif


struct umqttc_client *umqttc_new_ssl(const char *url, const char *ca_crt_file, bool verify)
{
    struct umqttc_client *cl = NULL;
    char *host = NULL;
    const char *path = "/";
    int port;
    int sock;
    bool ssl;

    if (parse_url(url, &host, &port, &path, &ssl) < 0) {
        umqttc_log_err("Invalid url");
        return NULL;
    }

    sock = usock(USOCK_TCP | USOCK_NOCLOEXEC, host, usock_port(port));
    if (sock < 0) {
        umqttc_log_err("usock");
        goto err;
    }

    cl = calloc(1, sizeof(struct umqttc_client));
    if (!cl) {
        umqttc_log_err("calloc");
        goto err;
    }

    cl->free = umqttc_free;

    ustream_fd_init(&cl->sfd, sock);

    if (ssl) {
#if (UWSC_SSL_SUPPORT)
        cl->ssl_ops = init_ustream_ssl();
        if (!cl->ssl_ops) {
            umqttc_log_err("SSL support not available,please install one of the libustream-ssl-* libraries");
            goto err;
        }

        cl->ssl_ctx = cl->ssl_ops->context_new(false);
        if (!cl->ssl_ctx) {
            umqttc_log_err("ustream_ssl_context_new");
            goto err;
        }

        if (ca_crt_file) {
            if (cl->ssl_ops->context_add_ca_crt_file(cl->ssl_ctx, ca_crt_file)) {
                umqttc_log_err("Load CA certificates failed");
                goto err;
            }
        } else if (verify) {
            int i;
            glob_t gl;

            cl->ssl_require_validation = true;

            if (!glob("/etc/ssl/certs/*.crt", 0, NULL, &gl)) {
                for (i = 0; i < gl.gl_pathc; i++)
                    cl->ssl_ops->context_add_ca_crt_file(cl->ssl_ctx, gl.gl_pathv[i]);
                globfree(&gl);
            }
        }

        cl->us = &cl->ussl.stream;
        cl->us->string_data = true;
        cl->us->notify_read = umqttc_ssl_notify_read;
        cl->us->notify_state = umqttc_ssl_notify_state;
        cl->ussl.notify_error = umqttc_ssl_notify_error;
        cl->ussl.notify_verify_error = umqttc_ssl_notify_verify_error;
        cl->ussl.notify_connected = umqttc_ssl_notify_connected;
        cl->ussl.server_name = host;
        cl->ssl_ops->init(&cl->ussl, &cl->sfd.stream, cl->ssl_ctx, false);
        cl->ssl_ops->set_peer_cn(&cl->ussl, host);
#else
        umqttc_log_err("SSL support not available");
        return NULL;
#endif
    } else {
        cl->us = &cl->sfd.stream;
        cl->us->string_data = true;
        cl->us->notify_read = umqttc_notify_read;
        cl->us->notify_state = umqttc_notify_state;
    }

    free(host);
    
    return cl;

err:
    if (host)
        free(host);

    if (cl)
        cl->free(cl);

    return NULL;    
}
