#include "AboutWindow.h"

namespace trview
{
    IAboutWindow::~IAboutWindow()
    {
    }

    void AboutWindow::focus()
    {
        _need_focus = true;
    }

    void AboutWindow::render()
    {
        if (_need_focus)
        {
            ImGui::SetNextWindowFocus();
            _need_focus = false;
        }

        bool stay_open = true;
        if (ImGui::Begin("About", &stay_open, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("trview, Version 2.7.3");
            ImGui::Text("Copyright (c) 2025 trview team");
            ImGui::Text("Licensed under the MIT license");
            ImGui::Text("\tUses DirectXTK (MIT)");
            ImGui::Text("\tUses zlib (zlib)");
            ImGui::Text("\tUses nlohmann/json (MIT)");
            ImGui::Text("\tUses Dear ImGui (MIT)");
            ImGui::Text("\tUses miniaudio (MIT)");
            ImGui::Text("Based on the information in TRosettaStone3 - https://github.com/opentomb/TRosettaStone3");
            ImGui::Text("Portions of this software are copyright (c) 2023 The FreeType Project (www.freetype.org). All rights reserved.");
        }
        ImGui::End();

        if (!stay_open)
        {
            on_window_closed();
        }
    }
}
