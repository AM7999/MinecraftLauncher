#include "Logger.hpp"

#include <SDL3/SDL_timer.h>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Xenia {
    static std::string current_time_hms() {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t tt = system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&tt);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        return oss.str();
    }

    void logMessage(std::string message) {
        std::cout << "(" << current_time_hms() << ") [LOG]: " << message << "\n";
    }
    void logWarning(std::string message) {
        std::cout << "(" << current_time_hms() << ") [WARN]: " << message << "\n";
    }
    void logError(std::string message, int eType) {
        if(eType == 1) {
            std::cout << "(" << current_time_hms() << ") [CRIT]: " << message << "\n";
            SDL_Delay(300);
            throw std::runtime_error(message);
        } else if(eType == 0) {
            std::cout << "(" << current_time_hms() << ") [ERR]: " << message << "\n";
        }
    }
}
