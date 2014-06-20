#ifndef __TIMING_H__
#define	__TIMING_H__

#include <windows.h>

typedef struct timer_t {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} timer_t;

void   timer_start(timer_t*);
void   timer_stop(timer_t*);
double timer_elapsed_seconds(timer_t*);
double timer_elapsed_miliseconds(timer_t*);

#endif	/* __TIMING_H__ */

