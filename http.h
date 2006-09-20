/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>

FILE * https_get(const char *host, const char *path);
void http_fire(FILE *f);
void http_auth_basic(FILE *f, const char *user, const char *pass);

#endif /* HTTP_H */
