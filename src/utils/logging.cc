#include "logging.h"
#include <cstdarg>
#include <cstdio>

void log(const char* format, ...) {
    std::fputs("[ERROR]", stderr);

    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);

    std::fputc('\n', stderr);
}
