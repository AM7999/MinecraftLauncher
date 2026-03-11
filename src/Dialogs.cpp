#include "Dialogs.hpp"

#include <imgui.h>

void Xenia::NewInstanceDialog(bool* p_open) {
    if (!p_open || !*p_open)
        return;

    static char txtBuff[32] = "";
    static int instanceType = 0;

    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("New Instance", p_open)) {

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
            *p_open = false;
        }
        if (!canCreate) ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 0))) {
            txtBuff[0] = '\0';
            *p_open = false;
        }
    }
    ImGui::End();
}