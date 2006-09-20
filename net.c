/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "net.h"

/**
 * \brief Inicializace.
 */
static void net_init() __attribute__((constructor));
static void net_init()
{
    SSL_library_init();
    SSL_load_error_strings();
}

/**
 * \brief Resolvuje IP, vraci ulong.
 * \return 0 pri chybe.
 */
unsigned long resolv(const char *host)
{
    struct hostent *hp;
    unsigned long host_ip = 0;

    hp = gethostbyname(host);
    if (!hp)
	fputs("Could not resolve hostname\n", stderr);
    else
	host_ip = *(unsigned long *)(hp->h_addr);

    return host_ip;
}

/**
 * \brief Pripojuje soket podle zadane adresy a portu, vraci pripojeny soket.
 * \return -1 pri chybe.
 */
int net_connect(const char *host, unsigned short port)
{
    unsigned long host_ip = resolv(host);
    if (!host_ip)
        goto chyba1;

    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        goto chyba1;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = host_ip;

    if (connect(sock, &sa, sizeof(sa)) == -1) {
        perror("connect");
        goto chyba2;
    }

    return sock;

chyba2:
    if (close(sock) == -1) {
        perror("close");
        abort();
    }
chyba1:
    return -1;
}

/**
 * \brief Cookie funkce pro uzavreni SSL spojeni i soketu.
 * \return Vzdy uspech - 0.
 */
int ssl_close(struct ssl_conn *conn)
{
    int ret;

    ret = SSL_shutdown(conn->ssl);
    if (ret == -1)
        ERR_print_errors_fp(stderr);

    SSL_free(conn->ssl);
    SSL_CTX_free(conn->ctx);

    if (close(conn->sock) == -1) {
        perror("close");
        abort();
    }

    return 0;
}

/**
 * \brief Cookie funkce pro cteni z SSL spojeni.
 */
ssize_t ssl_read(struct ssl_conn *conn, char *buf, size_t sz)
{
    int ret = SSL_read(conn->ssl, buf, sz);
    if (ret <= 0)
        ERR_print_errors_fp(stderr);

    return ret;
}

/**
 * \brief Cookie funkce pro zapis do SSL spojeni.
 */
ssize_t ssl_write(struct ssl_conn *conn, const char *buf, size_t sz)
{
    int ret = SSL_write(conn->ssl, buf, sz);
    if (ret <= 0)
        ERR_print_errors_fp(stderr);

    return ret;
}

/**
 * \brief Otevre SSL spojeni na danou adresu a port, vraci C stream.
 * \return 0 pri chybe.
 */
FILE * ssl_connect(const char *host, unsigned short port)
{
    struct ssl_conn *conn = malloc(sizeof(struct ssl_conn));
    if (!conn) {
        perror("malloc");
        abort();
    }

    conn->sock = net_connect(host, port);
    if (conn->sock == -1)
        goto chyba1;

    conn->ctx = SSL_CTX_new(SSLv23_client_method());
    if (!conn->ctx) {
        ERR_print_errors_fp(stderr);
        goto chyba2;
    }
    
    conn->ssl = SSL_new(conn->ctx);
    if (!conn->ssl) {
        ERR_print_errors_fp(stderr);
        goto chyba3;
    }

    if (!SSL_set_fd(conn->ssl, conn->sock)) {
        ERR_print_errors_fp(stderr);
        goto chyba4;
    }

    if (SSL_connect(conn->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        goto chyba4;
    }

    cookie_io_functions_t cfuncs;
    cfuncs.read = (cookie_read_function_t*) &ssl_read;
    cfuncs.write = (cookie_write_function_t*) &ssl_write;
    cfuncs.seek = 0;
    cfuncs.close = (cookie_close_function_t*) &ssl_close;

    FILE *f = fopencookie(conn, "rb+", cfuncs);
    if (!f) {
        perror("fopencookie");
        goto chyba4;
    }

    return f;

chyba4:
    SSL_free(conn->ssl);
chyba3:
    SSL_CTX_free(conn->ctx);
chyba2:
    if (close(conn->sock) == -1) {
        perror("close");
        abort();
    }
chyba1:
    free(conn);
    return 0;
}
