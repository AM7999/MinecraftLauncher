#ifndef LOGIC_H
#define LOGIC_H

#include "Structs.h"

namespace Xenia {
    class Logic {
        public:
            static void launchInstance(Instance& instance);
            static void modifyInstance(Instance& instance);
            static void DownloadMinecraft();
            //static Xenia::Instance createInstance(std::vector<Xenia::Instance>& instances);
    };
}

#endif