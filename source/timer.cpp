#include "timer.hpp"

size_t StartTimer(Timer* timer) {
    return timer->start = __rdtscp(&timer->ui);
}

size_t EndTimer(Timer* timer) {
    return timer->end = __rdtscp(&timer->ui);
}

size_t GetTime(Timer* timer) {
    return timer->end - timer->start;
}