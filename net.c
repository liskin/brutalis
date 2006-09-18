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
 * \brief Resolvuje IP, vraci ulong.
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
 */
int net_connect(const char *host, unsigned short port)
{
    unsigned long host_ip = resolv(host);
    if (!host_ip)
        abort();

    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        abort();
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = host_ip;

    if (connect(sock, &sa, sizeof(sa)) == -1) {
        perror("connect");
        abort();
    }

    return sock;
}

/**
 * \brief Cookie funkce pro uzavreni SSL spojeni i soketu.
 */
int ssl_close(struct ssl_conn *conn)
{
    int ret;

    /* Tohle nejak nefunguje nebo co. */
    /* while ((ret = SSL_shutdown(conn->ssl)) == 0);*/

    ret = SSL_shutdown(conn->ssl);
    if (ret == -1) {
        ERR_print_errors_fp(stderr);
        abort();
    }

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
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
    }

    return ret;
}

/**
 * \brief Cookie funkce pro zapis do SSL spojeni.
 */
ssize_t ssl_write(struct ssl_conn *conn, const char *buf, size_t sz)
{
    int ret = SSL_write(conn->ssl, buf, sz);
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
    }

    return ret;
}

/**
 * \brief Otevre SSL spojeni na danou adresu a port, vraci C stream.
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
        abort();

    conn->ctx = SSL_CTX_new(SSLv23_client_method());
    if (!conn->ctx) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    conn->ssl = SSL_new(conn->ctx);
    if (!conn->ssl) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if (!SSL_set_fd(conn->ssl, conn->sock)) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if (SSL_connect(conn->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    cookie_io_functions_t cfuncs;
    cfuncs.read = (cookie_read_function_t*) &ssl_read;
    cfuncs.write = (cookie_write_function_t*) &ssl_write;
    cfuncs.seek = 0;
    cfuncs.close = (cookie_close_function_t*) &ssl_close;

    FILE *f = fopencookie(conn, "rb+", cfuncs);
    if (!f) {
        perror("fopencookie");
        abort();
    }

    return f;
}
