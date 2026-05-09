#include "Window.hpp"

#include <iostream>
#include <fstream>
#include <ostream>
#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include "Dialogs.hpp"
#include "Logic.hpp"

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

        m_showSettings = false;
        m_showNewInstance = false;

        std::ifstream file;
        nlohmann::json j;
        file.open("launcherSettings.json");
        file >> j;
        instances = j.at("instances").get<std::vector<Xenia::Instance>>();
        jdks = j.at("installedJdks").get<std::vector<Xenia::JDK>>();
        clientSettings = j.at("userSettings").get<Xenia::clientSettings>();
        file.close();

        nlohmann::json j2;
        
        if(!std::filesystem::exists("cache/version_manifest.json")) {
            Logic::downloadFile("cache/", "https://launchermeta.mojang.com/mc/game/version_manifest.json");
        }
        file.open("cache/version_manifest.json");
        file >> j2;
        versions = j2.at("versions").get<std::vector<Xenia::version>>();
        file.close();

        //Logic::downloadMinecraft(versions[0], "test");
    }
    else {
        isRunning = false;
    }
}

Application::~Application() {
    // store everything back into json
    std::ifstream file("launcherSettings.json");
    nlohmann::json j;
    file >> j;
    file.close();

    j["instances"] = instances;
    j["installedJdks"] = jdks;
    j["userSettings"] = clientSettings;

    std::ofstream file2("launcherSettings.json");
    file2 << j.dump();
    file2.close();

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
            if (ImGui::MenuItem("Settings", "Ctrl + N")) { m_showSettings = true; }
            if (ImGui::MenuItem("New Instance", "Ctrl + N")) { m_showNewInstance = true; }
            if (ImGui::MenuItem("Quit.", "Ctrl + Q")) { isRunning = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(m_showNewInstance)
        Xenia::NewInstanceDialog(&m_showNewInstance, &instances, &versions);
    if (m_showSettings)
        Xenia::SettingsDialog(&m_showSettings, &clientSettings);

    // // Left panel - instance list
    ImGui::BeginGroup();

    float buttonHeight = ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("InstanceList", ImVec2(200, -buttonHeight), true);
    for (int i = 0; i < instances.size(); i++) {
        bool selected = (selectedIndex == i);
        if (ImGui::Selectable(instances[i].instanceName.c_str(), selected))
            selectedIndex = i;

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup(("InstanceContext##" + std::to_string(i)).c_str());

        if (ImGui::BeginPopup(("InstanceContext##" + std::to_string(i)).c_str())) {
            ImGui::Text("%s", instances[i].instanceName.c_str());
            ImGui::Separator();
            if (ImGui::MenuItem("Launch")) { /*Xenia::Logic::launchInstance(instances[i]);*/ }
            if (ImGui::MenuItem("Modify")) { /*Xenia::Logic::modifyInstance(instances[i]);*/ }
            if (ImGui::MenuItem("Delete Instance")) { instances.erase(instances.begin() + i); }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("New Instance", ImVec2(200, 0)))
        m_showNewInstance = true;

    ImGui::EndGroup();

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
        if (inst.isModded)
            if(ImGui::Button("Open mod folder", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                std::system(("xdg-open " + inst.pathToInstance + "/mods").c_str());
            }
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