#ifndef COMMON_TIME_H
#define COMMON_TIME_H

#include <time.h>
#include <stdbool.h>

long time_next_tick_ns(bool readonly);

#endif
