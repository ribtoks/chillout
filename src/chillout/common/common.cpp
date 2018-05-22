#include "common.h"
#include <ctime>
#include <locale>

tm now() {
    time_t now = time(0);
    return *localtime(&now);
}

std::ostream& formatDateTime(std::ostream& out, const tm& t, const char* fmt) {
    const std::time_put<char>& dateWriter = std::use_facet<std::time_put<char> >(out.getloc());
    const int n = strlen(fmt);
    dateWriter.put(out, out, ' ', &t, fmt, fmt + n);
    return out;
}
