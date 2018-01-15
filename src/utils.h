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
 
#ifndef _UTILS_H
#define _UTILS_H

#include <inttypes.h>
#include <stdbool.h>
#include "config.h"

#if (UMQTTC_SSL_SUPPORT)
#include <libubox/ustream-ssl.h>
#include <dlfcn.h>
#endif

int get_nonce(uint8_t *dest, int len);
int parse_url(const char *url, char **host, int *port, const char **path, bool *ssl);

#if (UMQTTC_SSL_SUPPORT)
const struct ustream_ssl_ops *init_ustream_ssl();
#endif

#endif
