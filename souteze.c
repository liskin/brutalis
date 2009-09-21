/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "net.h"
#include "http.h"
#include "time.h"

const char* parse_form(FILE *f, FILE *df);
char* URLencode(char* p, const char *str);

int main(int argc, char *argv[])
{
    /* UCO */
    char user[20];
    printf("Zadej UCO: ");
    if (scanf("%19s", user) != 1)
        abort();

    /* Cas */
    char cas[50], *cas_end;
    printf("Zadej cas: ");
    if (scanf("%49s", cas) != 1)
        abort();
    cas_end = cas + strlen(cas);

    struct tm lt;
    time_t now = time(0);
    localtime_r(&now, &lt);
    if (strptime(cas, "%T", &lt) != cas_end && strptime(cas, "%F_%T", &lt) != cas_end) {
        fputs("Zadal si spatne cas ty gumo.\n", stderr);
        abort();
    }

    lt.tm_isdst = -1;

    now = mktime(&lt);
    millitime_t t = (millitime_t) now * 1000;
    printf("Cekam do %s", ctime(&now));

    /* Soutez */
    int soutez;
    printf("Zadej cislo souteze: ");
    if (scanf("%d", &soutez) != 1)
        abort();

    /* Priprava debug filu. */
    char df_name[128];
    snprintf(df_name, 128, "brutalis-dbg-%s-XXXXXX", user);
    int df_fd = mkstemp(df_name);
    if (df_fd == -1)
        perror("mkstemp"), abort();
    FILE *df = fdopen(df_fd, "w");
    if (!df)
        perror("fdopen"), abort();
    printf("\033[1mVystup jde do: %s\033[m\n", df_name);

    brutalwait(t - 20000);

    FILE *f, *g;
    f = http_connect("souteze.is.muni.cz");
    g = http_connect("souteze.is.muni.cz");
    if (f && g) {
        /* Prvni pozadavek. */
        char path[1024];
        sprintf(path, "/?uco=%s&soutez=%i", user, soutez);
        http_get(f, "souteze.is.muni.cz", path);

        /* Druhy pozadavek. */
        http_post(g, "souteze.is.muni.cz", "/");

        /* Odeslani prvniho. */
        brutalwait(t);
        http_fire(f);

        /* Analyza prvniho. */
        const char *data2 = parse_form(f, df);
        fclose(f);

        fputs("\n\n\n\n\n\n", df);

        /* Odeslani druheho. */
        http_post_data_fire(g, data2);

        char buffer[4096];
        while (fgets(buffer, 4096, g)) {
            fputs(buffer, df);
        }

        fclose(g);
    }

    fclose(df);

    return 0;
}

char* app_formdata(char *data, char *d, const char *name, const char *value)
{
    if (d != data)
        d += sprintf(d, "&");
    d = URLencode(d, name);
    d += sprintf(d, "=");
    d = URLencode(d, value);

    return d;
}

const char* parse_form(FILE *f, FILE *df)
{
    static char data[4096];
    char *d = data;
    *d = 0;

    char buffer[4096], name[128], value[128];
    while (fgets(buffer, 4096, f)) {
        fputs(buffer, df); // debugfile

        char *p = buffer;
        while ((p = strstr(p, "<input type= hidden")))
            if (sscanf(p++, "<input type= hidden name=\"%127[^\"]\" value=\"%127[^\"]\">", name, value) == 2)
                d = app_formdata(data, d, name, value);
    }

    d = app_formdata(data, d, "soutezit", "x");

    return data;
}

int URLneedencode(unsigned char c)
{
    if (c <= 0x1f || c >= 0x7f)
        return 1;

    switch (c) {
        case 0x24:
        case 0x26:
        case 0x2b:
        case 0x2c:
        case 0x2f:
        case 0x3a:
        case 0x3b:
        case 0x3d:
        case 0x3f:
        case 0x40:
        case 0x20:
        case 0x22:
        case 0x3c:
        case 0x3e:
        case 0x23:
        case 0x25:
        case 0x7b:
        case 0x7d:
        case 0x7c:
        case 0x5c:
        case 0x5e:
        case 0x7e:
        case 0x5b:
        case 0x5d:
        case 0x60:
            return 1;
    }

    return 0;
}

char* URLencode(char* p, const char *str)
{
    while (*str)
        if (URLneedencode((unsigned char) *str))
            p += sprintf(p, "%%%.2hhX", (unsigned char) *str++);
        else
            p += sprintf(p, "%c", *str++);

    return p;
}
