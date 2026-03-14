#include "Dialogs.hpp"

#include <imgui.h>

#include "Structs.h"

void Xenia::NewInstanceDialog(bool* w_open) {
    if (!w_open || !*w_open)
        return;

    static char txtBuff[32] = "";
    int instanceType = Xenia::ModLoader::NONE;

    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("New Instance", w_open)) {

        ImGui::SetWindowFocus();

        ImGui::Spacing();
        ImGui::InputText("Instance Name", txtBuff, sizeof(txtBuff));

        ImGui::Spacing();
        ImGui::Text("Instance Type:");
        ImGui::RadioButton("Vanilla", &instanceType, 0);

        ImGui::Spacing();
        ImGui::Separator();

        bool canCreate = txtBuff[0] != '\0';

        if (!canCreate) ImGui::BeginDisabled();
        if (ImGui::Button("Create", ImVec2(80, 0))) {
            //CreateInstance(txtBuff, instanceType);
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

void Xenia::JdkDialog(bool *w_open) {
    if (!w_open || !*w_open)
        return;

    if (ImGui::Begin("Installed JDKs", w_open)) {
        ImGui::SetWindowFocus();

        ImGui::Spacing();


    }
}
