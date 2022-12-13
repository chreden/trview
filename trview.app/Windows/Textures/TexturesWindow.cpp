#include "TexturesWindow.h"
#include "../../Graphics/ILevelTextureStorage.h"
#include <format>

namespace trview
{
    ITexturesWindow::~ITexturesWindow()
    {
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
            ImGui::Checkbox(Names::transparency_checkbox.c_str(), &_transparency);

            const int32_t max_textures = std::max(0, (_texture_storage ? static_cast<int32_t>(_texture_storage->num_tiles()) : 0) - 1);
            if (ImGui::InputInt(Names::tile.c_str(), &_index))
            {
                _index = std::clamp(_index, 0, max_textures);
            }

            if (_texture_storage && _index < static_cast<int32_t>(_texture_storage->num_tiles()))
            {
                auto texture = _transparency ? _texture_storage->texture(_index) : _texture_storage->opaque_texture(_index);
                ImGui::Image(texture.view().Get(), ImVec2(256, 256));
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

    void TexturesWindow::set_texture_storage(const std::shared_ptr<ILevelTextureStorage>& texture_storage)
    {
        _texture_storage = texture_storage;
    }
}
