#include "Window.hpp"

#include <iostream>
#include <fstream>
#include <ostream>
#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include "Dialogs.hpp"

Application::Application() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        wnd = SDL_CreateWindow("Minecraft Launcher", 640, 480, 0);
        ren = SDL_CreateRenderer(wnd, "");
        if (!ren || !wnd)
            std::cerr << "Something broke: " << SDL_GetError() << std::endl;

        SDL_SetWindowResizable(wnd, false);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
        SDL_SetWindowPosition(wnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        isRunning = true;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL3_InitForSDLRenderer(wnd, ren);
        ImGui_ImplSDLRenderer3_Init(ren);

        m_showNewInstance = false;

        std::ifstream file("../launcherSettings.json");
        nlohmann::json j;
        file >> j;
        instances = j.at("instances").get<std::vector<Xenia::Instance>>();
    }
    else {
        isRunning = false;
    }
}

Application::~Application() {
    // store everything back into json
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(wnd);
    SDL_Quit();
}

void Application::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        switch(event.type) {
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;
            default:
                break;
        }
    }
}

void Application::draw() {
    SDL_RenderClear(ren);
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    static int selectedIndex = -1;

    ImGui::Begin("Main", nullptr, flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Instance", "Ctrl + N")) { m_showNewInstance = true; }
            if (ImGui::MenuItem("Quit.", "Ctrl + Q")) { isRunning = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(m_showNewInstance)
        Xenia::NewInstanceDialog(&m_showNewInstance);

    // Left panel - instance list
    ImGui::BeginChild("InstanceList", ImVec2(200, 0), true);
    for (int i = 0; i < instances.size(); i++) {
        bool selected = (selectedIndex == i);
        if (ImGui::Selectable(instances[i].instanceName.c_str(), selected))
            selectedIndex = i;

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup(("InstanceContext##" + std::to_string(i)).c_str());

        if (ImGui::BeginPopup(("InstanceContext##" + std::to_string(i)).c_str())) {
            ImGui::Text(instances[i].instanceName.c_str());
            ImGui::Separator();
            if (ImGui::MenuItem("Launch")) { /* stub */ }
            if (ImGui::MenuItem("Modify")) { /* stub */ }
            if (ImGui::MenuItem("Delete Instance")) { /* stub */ }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel - instance details
    ImGui::BeginChild("InstanceDetails", ImVec2(0, 0), true);
    if (selectedIndex >= 0 && selectedIndex < instances.size()) {
        Xenia::Instance& inst = instances[selectedIndex];
        ImGui::Text("Name: %s",        inst.instanceName.c_str());
        ImGui::Text("Version: %s",     inst.minecraftVersion.c_str());
        ImGui::Text("Java: %d",        inst.javaVersion);
        ImGui::Text("Modded: %s",      inst.isModded ? "Yes" : "No");
        if (inst.isModded)
            ImGui::Text("Mod Loader: %s", Xenia::ModLoaderToString(inst.ml).c_str());
        ImGui::Text("Path: %s",        inst.pathToInstance.c_str());
        if (ImGui::Button("Open Instance Folder", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            std::system(("xdg-open " + inst.pathToInstance).c_str());
        }
        ImGui::Button("Play!", ImVec2(ImGui::GetContentRegionAvail().x, 0));
    } else {
        ImGui::TextDisabled("Select an instance to view details.");
    }
    ImGui::EndChild();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), ren);

    SDL_RenderPresent(ren);
}