#include "TexturesWindow.h"
#include "../../Messages/Messages.h"
#include "../../Elements/ILevel.h"
#include <format>

namespace trview
{
    TexturesWindow::TexturesWindow(const std::weak_ptr<IMessageSystem>& messaging)
        : _messaging(messaging)
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

    void TexturesWindow::update(float)
    {
    }

    bool TexturesWindow::render_textures_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(270, 335));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_NoResize))
        {
            ImGui::Checkbox(Names::transparency_checkbox.c_str(), &_transparency);

            if (ImGui::InputInt(Names::tile.c_str(), &_index))
            {
                clamp_index();
            }

            if (_texture_storage && _index < static_cast<int32_t>(_texture_storage->num_tiles()))
            {
                auto texture = _transparency ? _texture_storage->texture(_index) : _texture_storage->opaque_texture(_index);
                ImGui::Image(texture.view().Get(), ImVec2(texture.size().width, texture.size().height));
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
        clamp_index();
    }

    void TexturesWindow::clamp_index()
    {
        const int32_t max_textures = std::max(0, (_texture_storage ? static_cast<int32_t>(_texture_storage->num_tiles()) : 0) - 1);
        _index = std::clamp(_index, 0, max_textures);
    }

    void TexturesWindow::receive_message(const Message& message)
    {
        if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                set_texture_storage(level_ptr->texture_storage());
            }
        }
    }

    void TexturesWindow::initialise()
    {
        messages::get_open_level(_messaging, weak_from_this());
    }

    std::string TexturesWindow::type() const
    {
        return "Textures";
    }

    std::string TexturesWindow::title() const
    {
        return _id;
    }
}
