#ifndef LOGGING
#define LOGGING

void log(const char* format, ...);

#ifdef ENABLE_LOGGING
    #define LOG_ERROR(...) log(__VA_ARGS__)
#else
    #define LOG_ERROR(...) ((void)0)
#endif
#endif
