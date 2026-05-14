#include <iostream>
#include <fstream>
#include <SDL3/SDL_messagebox.h>
#include "Window.hpp"

using nlohmann::json;

constexpr int targetFps = 60;
constexpr int frameDelay = 1000/targetFps;

Uint32 frameStart;
int frameTime;

int main(int argc, char *argv[]) {
    if (!std::filesystem::exists("launcherSettings.json")) {
        std::ofstream outFile("launcherSettings.json");

        if (!outFile.is_open()) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create config file.", NULL);
            return -1;
        }

        json settings = {
            {"userSettings", {
                {"username", "Player"},
                {"online", false},
                {"maxMemoryAlloc", 512}
            }},
            {"installedJdks", json::array()},
            {"instances", json::array()}
        };

        outFile << settings.dump();
        outFile.close();

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", "A config has been created. Please restart the launcher.", NULL);
        return 1;
    }

    if (!std::filesystem::exists("cache")) { std::filesystem::create_directory("cache"); }
    if (!std::filesystem::exists("Instances")) { std::filesystem::create_directory("Instances"); }

    Application* a = new Application();

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
