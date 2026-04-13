#include <stdlib.h>
#include <x86intrin.h>

struct Timer {
    size_t start;
    size_t end;
    unsigned int ui;
};

size_t StartTimer(Timer* timer);
size_t EndTimer(Timer* timer);
size_t GetTime(Timer* timer);