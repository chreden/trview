#include "AboutWindow.h"
#include <trview.common/Version.h>

namespace trview
{
    void AboutWindow::update(float)
    {
    }

    void AboutWindow::render()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 0));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_None))
        {
            ImGui::TextWrapped(std::format("trview, Version {}", trview::version()).c_str());
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
            ImGui::TextWrapped("TR4/5 PSX loading based on TOMB5 - https://github.com/TOMB5/TOMB5");
        }
        ImGui::End();
        ImGui::PopStyleVar();

        if (!stay_open)
        {
            on_window_closed();
        }
    }

    void AboutWindow::set_number(int32_t number)
    {
        _id = std::format("About {}", number);
    }

    std::string AboutWindow::type() const
    {
        return "About";
    }

    void AboutWindow::receive_message(const Message&)
    {
    }
}
