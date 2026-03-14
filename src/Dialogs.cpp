#include "Dialogs.hpp"

#include <imgui.h>

#include "Structs.h"

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

void Xenia::NewInstanceDialog(bool* w_open, std::vector<Xenia::Instance>* instances) {
    if (!w_open || !*w_open)
        return;

    static char txtBuff[32] = "";
    static int instanceType = Xenia::ModLoader::NONE;
    static bool modded = false;

    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("New Instance", w_open)) {

        ImGui::SetWindowFocus();

        ImGui::Spacing();
        ImGui::InputText("Instance Name", txtBuff, sizeof(txtBuff));

        ImGui::Spacing();
        ImGui::Text("Instance Type:");
        ImGui::Checkbox("Modded", &modded);
        ImGui::RadioButton("Neoforge", &instanceType, Xenia::ModLoader::NEOFORGE);
        ImGui::SameLine();
        ImGui::RadioButton("Forge", &instanceType, Xenia::ModLoader::FORGE);

        ImGui::Spacing();
        ImGui::Separator();

        bool canCreate = txtBuff[0] != '\0';

        if (!canCreate) ImGui::BeginDisabled();
        if (ImGui::Button("Create", ImVec2(80, 0))) {
            instances->push_back({txtBuff, "1.21.1", 21, modded, Xenia::intToModloader(instanceType), "/path"});
            txtBuff[0] = '\0';
            *w_open = false;
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
