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
#include <unistd.h>
#include <time.h>
#include "net.h"
#include "http.h"
#include "time.h"

int main(int argc, char *argv[])
{
    char user[20];
    printf("Zadej UCO: ");
    scanf("%19s", user);

    char *pass = strdup(getpass("Zadej heslo: "));
    if (strcmp(pass, getpass("Znovu heslo: "))) {
        fputs("Hesla se lisi.\n", stderr);
        abort();
    }

    char path[512];
    printf("Zadej cestu: ");
    scanf("%511s", path);

    char cas[20];
    printf("Zadej cas: ");
    scanf("%19s", cas);

    struct tm lt;
    time_t now = time(0);
    localtime_r(&now, &lt);
    if (!strptime(cas, "%T", &lt)) {
        fputs("Zadal si spatne cas ty gumo.\n", stderr);
        abort();
    }

    lt.tm_isdst = -1;

    millitime_t t = (millitime_t) mktime(&lt) * 1000;

    brutalwait(t - 20000);

    FILE *f = https_get("is.muni.cz", path);
    if (f) {
        http_auth_basic(f, user, pass);

        brutalwait(t);

        http_fire(f);

        int ok = 0;

        char buffer[4096];
        while (fgets(buffer, 4096, f)) {
            if (strstr(buffer, "Potvrzení:") || strstr(buffer, "Varování:"))
                ok = 1;
            //fputs(buffer, stdout);
        }

        fclose(f);

        if (!ok)
            printf("\033[1;31mCHYBA\033[m\n");
        else
            printf("\033[1;32mOK\033[m\n");
    }

    free(pass);

    return 0;
}
