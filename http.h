#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>

FILE * http_get(const char *host, const char *path);
void http_fire(FILE *f);
void http_auth_basic(FILE *f, const char *user, const char *pass);

#endif /* HTTP_H */
