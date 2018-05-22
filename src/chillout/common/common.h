#ifndef COMMON_H
#define COMMON_H

#include <ctime>
#include <ostream>

#define CHILLOUT_DATETIME "%Y%m%d_%H%M%S"

tm now();
std::ostream& formatDateTime(std::ostream& out, const tm& t, const char* fmt);

#endif // COMMON_H
