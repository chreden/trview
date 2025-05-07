#include "PackWindow.h"
#include <trlevel/Level.h>

namespace trview
{
    IPackWindow::~IPackWindow()
    {
    }

    PackWindow::PackWindow(const std::shared_ptr<IFiles>& files, const std::shared_ptr<IDialogs>& dialogs)
        : _files(files), _dialogs(dialogs)
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
                        if (ImGui::BeginPopupContextItem())
                        {
                            if (ImGui::MenuItem("Save"))
                            {
                                if (const auto file = _dialogs->save_file(std::format(L"Saving {}", part.start),
                                    { { L"TR4 levels", { L"*.tr4" } }, { L"TR5 levels", { L"*.trc" }}, { L"All files", { L"*.*" }} },
                                    part.version.has_value() ? (part.version.value().version == trlevel::LevelVersion::Tomb4 ? 1 : 2) : 3))
                                {
                                    _files->save_file(file->filename, part.data);
                                }
                            }
                            ImGui::EndPopup();
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(part.size).c_str());
                        ImGui::TableNextColumn();
                        if (part.version)
                        {
                            ImGui::Text(trlevel::to_string(part.version->version).c_str());
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

