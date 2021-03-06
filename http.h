/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>

FILE * http_connect(const char *host);
FILE * https_connect(const char *host);
void http_get(FILE *f, const char *host, const char *path);
void http_post(FILE *f, const char *host, const char *path);
void http_post_data_fire(FILE *f, const char *data);
void http_fire(FILE *f);
void http_auth_basic(FILE *f, const char *user, const char *pass);
void http_cookie(FILE *f, const char *name, const char *value);

#endif /* HTTP_H */
