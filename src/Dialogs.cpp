#include "Dialogs.hpp"

#include <imgui.h>
#include <future>

#include "Structs.h"
#include "Logic.hpp"

void Xenia::SettingsDialog(bool* w_open, Xenia::clientSettings* cs) {
    if (!w_open || !*w_open)
        return;

    static char txtBuff[16] = "";
    static int memory = 0;

    if (ImGui::Begin("Settings", w_open)) {
        ImGui::SetWindowFocus();

        ImGui::Spacing();

        ImGui::Text("Current Username: %s", cs->username.c_str());
        ImGui::Text("Current Memory: %s", std::to_string(cs->memory).c_str());

        ImGui::Spacing();

        ImGui::InputText("Username", txtBuff, sizeof(txtBuff));

        ImGui::Spacing();
        ImGui::InputInt("Label", &memory, 1, 100);

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Save", ImVec2(80, 0))) {
            cs->username = txtBuff;
            cs->memory = memory;
            memory = 0;
            txtBuff[0] = '\0';
            *w_open = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 0))) {
            memory = 0;
            txtBuff[0] = '\0';
            *w_open = false;
        }
    }
    ImGui::End();
}

void Xenia::NewInstanceDialog(bool* w_open, std::vector<Xenia::Instance>* instances, std::vector<Xenia::version>* v) {
    if (!w_open || !*w_open)
        return;

    static int selectedIndex = 0;
    static char txtBuff[32] = "";
    static int instanceType = Xenia::ModLoader::NONE;
    static bool modded = false;
    static int selectedVersionIndex = 0;

    if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId))
        ImGui::SetNextWindowFocus();

    ImGui::SetNextWindowSize(ImVec2(300, 220), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("New Instance", w_open)) {

        ImGui::Spacing();
        ImGui::InputText("Instance Name", txtBuff, sizeof(txtBuff));

        ImGui::Spacing();
        if (v && !v->empty()) {
            if (selectedVersionIndex >= static_cast<int>(v->size()))
                selectedVersionIndex = 0;

            const auto getter = [](void* data, int idx) -> const char* {
                auto* versions = static_cast<std::vector<Xenia::version>*>(data);
                return (*versions)[idx].id.c_str();
            };
            ImGui::Combo("Version", &selectedVersionIndex, getter, v, static_cast<int>(v->size()));
        } else {
            ImGui::TextDisabled("No versions available");
        }

        ImGui::BeginDisabled();
        ImGui::Spacing();
        ImGui::Text("Instance Type:");
        ImGui::Checkbox("Modded", &modded);
        ImGui::RadioButton("Neoforge", &instanceType, Xenia::ModLoader::NEOFORGE);
        ImGui::SameLine();
        ImGui::RadioButton("Forge", &instanceType, Xenia::ModLoader::FORGE);
        ImGui::SameLine();
        ImGui::RadioButton("Fabric", &instanceType, Xenia::ModLoader::FABRIC);
        ImGui::SameLine();
        ImGui::RadioButton("Quilt", &instanceType, Xenia::ModLoader::QUILT);
        ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Separator();

        bool canCreate = txtBuff[0] != '\0' && v && !v->empty();

        if (!canCreate) ImGui::BeginDisabled();
        if (ImGui::Button("Create", ImVec2(80, 0))) {
            const Xenia::version& selected = (*v)[selectedVersionIndex];
            Logic::downloadMinecraft(selected, txtBuff); //Logic::downloadMinecraft(selected, txtBuff);
            instances->push_back({txtBuff, selected.id, 21, modded, Xenia::intToModloader(instanceType), txtBuff});
            *w_open = false;
            txtBuff[0] = '\0';
        }
        if (!canCreate) ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 0))) {
            txtBuff[0] = '\0';
            *w_open = false;
        }
    }
    ImGui::End();
}

void Xenia::JdkDialog(bool *w_open, std::vector<Xenia::JDK>* jdks) {
    if (!w_open || !*w_open)
        return;

    if (ImGui::Begin("Installed JDKs", w_open)) {
        ImGui::SetWindowFocus();

        ImGui::Spacing();


    }
}
