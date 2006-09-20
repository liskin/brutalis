/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "time.h"

/**
 * \brief Vraci cas od epoch v milisekundach.
 */
millitime_t millitime()
{
    struct timeval tv;
    if (gettimeofday(&tv, 0)) {
        perror("gettimeofday");
        abort();
    }

    return (millitime_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * \brief Ceka do dane cilove doby a ani o chvili dele.
 */
void brutalwait(millitime_t target)
{
    millitime_t t;

    while ((t = millitime()) < target) {
        usleep((target - t) * 800);
    }
}
