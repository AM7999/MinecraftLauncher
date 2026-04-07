#ifndef LOGIC_H
#define LOGIC_H

#include "Structs.h"

namespace Logic {
    bool downloadFile(const std::string& directory, const std::string& url);
    bool downloadMinecraft(const Xenia::version& v, const std::string& instanceName);
    bool generateLaunchScript(const Xenia::Instance& instance, const Xenia::JDK& jdk);
}

#endif