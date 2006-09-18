#include <string.h>
#include "http.h"
#include "net.h"
#include "base64.h"

FILE * http_get(const char *host, const char *path)
{
    FILE *f = ssl_connect(host, 443);
    if (!f)
        return 0;

    fprintf(f, "GET %s HTTP/1.0\r\n"
            "User-Agent: brutalis/3.14\r\n"
            "Host: %s\r\n"
            "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n"
            "Accept-Charset: utf-8;q=0.7,*;q=0.7\r\n"
            "Connection: close\r\n",
            path, host);

    return f;
}

void http_fire(FILE *f)
{
    fputs("\r\n", f);
}

void http_auth_basic(FILE *f, const char *user, const char *pass)
{
    char blah[strlen(user) + strlen(pass) + 2];

    strcpy(blah, user);
    strcat(blah, ":");
    strcat(blah, pass);

    char *b64 = (char *) base64_encode((const unsigned char *) blah, strlen(blah), 0);

    fprintf(f, "Authorization: Basic %s\r\n", b64);

    free(b64);
}
