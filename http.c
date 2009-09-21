/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#include <string.h>
#include "http.h"
#include "net.h"
#include "base64.h"

/**
 * \brief Vytvori nove HTTP spojeni.
 */
FILE * http_connect(const char *host)
{
    return plain_connect(host, 80);
}

/**
 * \brief Vytvori nove HTTPS spojeni.
 */
FILE * https_connect(const char *host)
{
    return ssl_connect(host, 443);
}

/**
 * \brief Posle zacatek pozadavku.
 */
static void http_req(FILE *f, const char *req, const char *host, const char *path)
{
    fprintf(f, "%s %s HTTP/1.0\r\n"
            "User-Agent: brutalis/3.14\r\n"
            "Host: %s\r\n"
            "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n"
            "Accept-Charset: utf-8;q=0.7,*;q=0.7\r\n"
            "Connection: close\r\n",
            req, path, host);
}

/**
 * \brief Posle zacatek GET pozadavku.
 */
void http_get(FILE *f, const char *host, const char *path)
{
    http_req(f, "GET", host, path);
}

/**
 * \brief Posle zacatek POST pozadavku.
 */
void http_post(FILE *f, const char *host, const char *path)
{
    http_req(f, "POST", host, path);
    fprintf(f, "Content-type: application/x-www-form-urlencoded\r\n");
}

/**
 * \brief Posle POST data.
 */
void http_post_data_fire(FILE *f, const char *data)
{
    fprintf(f, "Content-length: %u\r\n" "\r\n" "%s", strlen(data), data);
    fflush(f);
}

/**
 * \brief Odpali pozadavek.
 */
void http_fire(FILE *f)
{
    fputs("\r\n", f);
}

/**
 * \brief Prida HTTP Basic auth. hlavicku.
 */
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
