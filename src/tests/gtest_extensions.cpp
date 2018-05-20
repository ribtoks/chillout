#include "gtest_extensions.h"

namespace extensions {

ExitedOrKilled::ExitedOrKilled(int exit_code, int signal):
    _exitedWithCode(exit_code),
    _killedBySignal(signal)
{ }

bool ExitedOrKilled::operator()(int value) const {
    return _exitedWithCode(value) ||
        _killedBySignal(value);
}
}
