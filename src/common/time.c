#include "time.h"

#include <time.h>

#define TICK_RATE 64
#define TICK_DURATION_NS (1000000000L / TICK_RATE)
#define MAX(a, b) (a > b ? a : b)

long time_next_tick_ns(bool readonly) {
	static struct timespec last;

	struct timespec now;
	timespec_get(&now, TIME_UTC);

	long elapsed = (now.tv_sec - last.tv_sec) * 1000000000L
	             + (now.tv_nsec - last.tv_nsec);

	long remaining = TICK_DURATION_NS - elapsed;
	if (!readonly && remaining < 0)
		last = now;

	return MAX(0, remaining);
}
