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

int main(int argc, char *argv[])
{
    /* UCO */
    char user[20];
    printf("Zadej UCO: ");
    scanf("%19s", user);

    /* Heslo */
    char iscreds[256];
    printf("Zadej iscreds: ");
    scanf("%255s", iscreds);

    /* Cas */
    char cas[50], *cas_end;
    printf("Zadej cas: ");
    scanf("%49s", cas);
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

    /* Pocet pokusu */
    int pocet_pokusu = 1;
    printf("Zadej pocet pokusu: ");
    scanf("%d", &pocet_pokusu);

    pid_t pid = 1;

    /* Cesty */
    char path[512];
    while (1) {
        usleep(100000);
        printf("Zadej cestu: ");
        if (scanf("%511s", path) != 1) {
            printf("Ukoncuji zadani. Vyckejte provedeni uloh.\n");
            break;
        }

        /* Child nepokračuje ve smyčce */
        if ((pid = fork()) == 0)
            break;
    }

    /* Jen childi pracují */
    if (pid == 0) {
        /* Naforkovat na počet pokusů */
        pid = 1;
        while (pid > 0 && pocet_pokusu-- > 1)
            pid = fork();

        /* Priprava debug filu. */
        char df_name[128];
        snprintf(df_name, 128, "brutalis-dbg-%s-XXXXXX", user);
        int df_fd = mkstemp(df_name);
        if (df_fd == -1)
            perror("mkstemp"), abort();
        FILE *df = fdopen(df_fd, "w");
        if (!df)
            perror("fdopen"), abort();
        fprintf(df, "PATH: %s\n\n", path);
        printf("\033[1mVystup jde do: %s\033[m\n", df_name);

        brutalwait(t - 20000);

        FILE *f = https_connect("is.muni.cz");
        if (f) {
            http_get(f, "is.muni.cz", path);
            http_cookie(f, "iscreds", iscreds);

            brutalwait(t);

            http_fire(f);

            int ok = 0;

            char cas[512] = "", *cas_p;
            static const char cas_str[] = "Čas Informačního systému: <B>";

            char buffer[4096];
            while (fgets(buffer, 4096, f)) {
                fputs(buffer, df);

                if (strstr(buffer, "potvrzeni") || strstr(buffer, "varovani"))
                    ok = 1;

                if ((cas_p = strstr(buffer, cas_str))) {
                    cas_p += strlen(cas_str);

                    char *end = strstr(cas_p, "</B>");
                    if (end)
                        *end = 0;

                    strncpy(cas, cas_p, 512);
                    cas[511] = 0;
                }

                //fputs(buffer, stdout);
            }

            fclose(f);

            if (!ok)
                printf("%s: \033[1;31mCHYBA\033[m\n", cas);
            else
                printf("%s: \033[1;32mOK\033[m\n", cas);
        }

        fclose(df);
    }

    /* Pockat na deti. */
    if (pid > 0)
        while (wait(0) > 0);

    return 0;
}
