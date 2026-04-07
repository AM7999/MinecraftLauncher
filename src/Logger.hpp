#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace Xenia {
    void logMessage(std::string message);
    void logError(std::string message, int eType);
    void logWarning(std::string message);
}

#endif
