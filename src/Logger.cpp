#include "Logger.hpp"

#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

#define WHITE "\x1b[0;38;2;255;255;255;49m"
#define GREEN "\x1b[0;38;2;28;220;154;49m"
#define ORANGE "\x1b[0;38;2;225;85;0;49m"
#define RED "\x1b[0;38;2;255;0;0;49m"

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
        std::cout << WHITE << "(" << current_time_hms() << ")" << GREEN << "[LOG]: " << WHITE << message << "\n";
    }
    void logWarning(std::string message) {
        std::cout << WHITE << "(" << current_time_hms() << ")" << ORANGE <<  "[WARN]: " << WHITE << message << "\n";
    }
    void logError(std::string message, int eType) {
        if(eType == 1) {
            std::cout << WHITE << "(" << current_time_hms() << ")" << RED << "[CRIT]: " << RED << message << "\n";
        } else if(eType == 0) {
            std::cout << WHITE << "(" << current_time_hms() << ")" << RED << "[ERR]: " << WHITE << message << "\n";
        }
    }
}
