#ifndef DIALOG_H
#define DIALOG_H
#include "Structs.h"

namespace Xenia {
    void SettingsDialog(bool* w_open, Xenia::clientSettings* cs);
    void NewInstanceDialog(bool* w_open, std::vector<Xenia::Instance>* instances);
    void JdkDialog(bool* w_open, std::vector<Xenia::JDK>* jdks);
}

#endif