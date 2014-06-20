#include "timing.h"

inline void timer_start(timer_t *self) {
    QueryPerformanceCounter(&(self->start));
}

inline void timer_stop(timer_t *self) {
    QueryPerformanceCounter(&(self->stop));
}

double timer_elapsed_seconds(timer_t *self) {
    LARGE_INTEGER interval, frequency;
    interval.QuadPart = self->stop.QuadPart - self->start.QuadPart; 
    QueryPerformanceFrequency(&frequency);
    return ((double)(interval.QuadPart)) / ((double)(frequency.QuadPart));
}

inline double timer_elapsed_miliseconds(timer_t *self) {
    return timer_elapsed_seconds(self) * 1000.0;
}
