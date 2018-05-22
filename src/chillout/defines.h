#ifndef DEFINES_H
#define DEFINES_H

#define CHILLOUT_EXIT_CODE 3

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

#ifdef _WIN32
#define WIDEN(quote) WIDEN2(quote)
#define WIDEN2(quote) L##quote
#else
#define WIDEN(quote) quote
#endif

#endif // DEFINES_H
