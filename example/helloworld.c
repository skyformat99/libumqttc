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

#include <umqttc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct umqttc_client *gcl;

static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [option]\n"
        "      -u url       # ws://localhost:8080/ws\n"
        "      -c file      # Load CA certificates from file\n"
        "      -n           # don't validate the server's certificate\n"
        , prog);
    exit(1);
}

int main(int argc, char **argv)
{
    int opt;
    static bool verify = true;
    const char *url = "ws://localhost:8080/ws";
    const char *crt_file = NULL;

    while ((opt = getopt(argc, argv, "u:nc:")) != -1) {
        switch (opt)
        {
        case 'u':
            url = optarg;
            break;
        case 'n':
            verify = false;
            break;
        case 'c':
            crt_file = optarg;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    uloop_init();

    gcl = umqttc_new_ssl(url, crt_file, verify);
    if (!gcl) {
        uloop_done();
        return -1;
    }
   
    uloop_run();

    gcl->free(gcl);

    uloop_done();
    
    return 0;
}
