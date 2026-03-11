#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include <vector>

#include <imgui.h>

#include "Structs.h"

class Application {
    public:
        Application();
        ~Application();
        void handleEvents();
        //void update();
        void draw();

        bool getIsRunning() { return isRunning; }
    private:
        SDL_Window* wnd;
        SDL_Renderer* ren;
        bool isRunning;
        std::vector<Xenia::Instance> instances;
        std::vector<Xenia::JDK> jdks;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
        bool m_showNewInstance{};
};

#endif