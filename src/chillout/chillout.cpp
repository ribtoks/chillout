#include "chillout.h"

#ifdef _WIN32
#include "windows/windowscrashhandler.h"
#endif

void Chillout::init()
{
#ifdef _WIN32
    WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
    handler.Setup();
#endif
}



