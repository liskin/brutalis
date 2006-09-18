#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/**
 * \brief Cookie struktura pro SSL spojeni.
 */
struct ssl_conn {
    int sock;
    SSL_CTX *ctx;
    SSL *ssl;
};

unsigned long resolv(const char *host);
int net_connect(const char *host, unsigned short port);
FILE * ssl_connect(const char *host, unsigned short port);

#endif /* NET_H */