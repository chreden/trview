#include "PackWindow.h"
#include <trlevel/Level.h>

namespace trview
{
    IPackWindow::~IPackWindow()
    {
    }

    void PackWindow::render()
    {
        if (!render_pack_window())
        {
            on_window_closed();
            return;
        }
    }

    void PackWindow::set_number(int32_t number)
    {
        _index = number;
    }

    void PackWindow::set_pack(const std::weak_ptr<trlevel::IPack>& pack)
    {
        _pack = pack;
    }

    bool PackWindow::render_pack_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(270, 335));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            auto pack = _pack.lock();
            if (pack)
            {
                if (ImGui::BeginTable("PartsList", 3))
                {
                    ImGui::TableSetupColumn("Offset");
                    ImGui::TableSetupColumn("Size");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupScrollFreeze(1, 1);
                    ImGui::TableHeadersRow();

                    uint32_t index = 0;
                    for (const auto& part : pack->parts())
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool selected = false;
                        if (ImGui::Selectable(std::format("{}##{}", part.start, index++).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                        {
                            on_level_open(std::format("pack://{}\\{}", pack->filename(), part.start));
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(part.size).c_str());
                        ImGui::TableNextColumn();
                        if (part.level)
                        {
                            ImGui::Text(trlevel::to_string(part.level->platform_and_version().version).c_str());
                        }
                    }

                    ImGui::EndTable();
                }
            }
            else
            {
                ImGui::Text("There is no pack");
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }
}

