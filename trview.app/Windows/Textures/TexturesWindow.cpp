#include "TexturesWindow.h"
#include "../../Graphics/ILevelTextureStorage.h"
#include <format>
#include "../../Elements/ILevel.h"

namespace trview
{
    ITexturesWindow::~ITexturesWindow()
    {
    }

    void TexturesWindow::add_level(const std::weak_ptr<ILevel>& level)
    {
        _sub_windows.push_back({ ._level = level });
    }

    void TexturesWindow::render()
    {
        if (!render_textures_window())
        {
            on_window_closed();
            return;
        }
    }

    bool TexturesWindow::render_textures_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(270, 335));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_NoResize))
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                int window_index = 0;
                for (auto& sub_window : _sub_windows)
                {
                    sub_window.render(window_index++);
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void TexturesWindow::set_number(int32_t number)
    {
        _id = std::format("Textures {}", number);
    }

    void TexturesWindow::SubWindow::render(int index)
    {
        if (auto level = _level.lock())
        {
            if (const auto texture_storage = level->texture_storage())
            {
                if (ImGui::BeginTabItem(std::format("{}##{}", level->name(), index).c_str()))
                {
                    ImGui::Checkbox(Names::transparency_checkbox.c_str(), &_transparency);

                    if (ImGui::InputInt(Names::tile.c_str(), &_index))
                    {
                        const int32_t max_textures = std::max(0, (texture_storage ? static_cast<int32_t>(texture_storage->num_tiles()) : 0) - 1);
                        _index = std::clamp(_index, 0, max_textures);
                    }

                    if (texture_storage && _index < static_cast<int32_t>(texture_storage->num_tiles()))
                    {
                        auto texture = _transparency ? texture_storage->texture(_index) : texture_storage->opaque_texture(_index);
                        ImGui::Image(texture.view().Get(), ImVec2(256, 256));
                    }

                    ImGui::EndTabItem();
                }
            }
        }
    }
}
