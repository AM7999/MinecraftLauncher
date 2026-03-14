#include <iostream>
#include <fstream>

#include "Structs.h"
#include "Window.hpp"

using nlohmann::json;

Application* a = new Application();

constexpr int targetFps = 60;
constexpr int frameDelay = 1000/targetFps;

Uint32 frameStart;
int frameTime;

int main(int argc, char *argv[]) {
    while (a->getIsRunning()) {
        frameStart = SDL_GetTicks();

        a->handleEvents();
        a->draw();

        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);
    }

    delete a;

    return 0;
}
