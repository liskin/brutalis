/*
 * Brutalis, is.muni.cz automatzor.
 * Copyright (c) 2006, Tomas Janousek <tomi@nomi.cz>
 *
 * Tohle je neverejny a ve vlastnim zajmu to nikomu nedavejte.
 */

#ifndef TIME_H
#define TIME_H

typedef long long millitime_t;

millitime_t millitime();
void brutalwait(millitime_t target);

#endif /* TIME_H */
