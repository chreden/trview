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
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 0));
        if (ImGui::Begin("About", &stay_open, ImGuiWindowFlags_None))
        {
            ImGui::TextWrapped("trview, Version 2.7.3");
            ImGui::TextWrapped("Copyright (c) 2025 trview team");
            ImGui::TextWrapped("Licensed under the MIT license");
            ImGui::TextWrapped("Uses DirectXTK (MIT)");
            ImGui::TextWrapped("Uses zlib (zlib)");
            ImGui::TextWrapped("Uses nlohmann/json (MIT)");
            ImGui::TextWrapped("Uses Dear ImGui (MIT)");
            ImGui::TextWrapped("Uses miniaudio (MIT)");
            ImGui::TextWrapped("PSX audio code based on vag2wav from vag2wav from http://unhaut.epizy.com/psxsdk/");
            ImGui::TextWrapped("Based on the information in TRosettaStone3 - https://github.com/opentomb/TRosettaStone3");
            ImGui::TextWrapped("Portions of this software are copyright (c) 2023 The FreeType Project (www.freetype.org). All rights reserved.");
        }
        ImGui::End();
        ImGui::PopStyleVar();

        if (!stay_open)
        {
            on_window_closed();
        }
    }
}
