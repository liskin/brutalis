/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

FILE * plain_connect(const char *host, unsigned short port);
FILE * ssl_connect(const char *host, unsigned short port);

#endif /* NET_H */
