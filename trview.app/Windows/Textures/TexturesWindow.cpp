#include "TexturesWindow.h"
#include "../../Graphics/ILevelTextureStorage.h"
#include <format>
#include <external/DirectXTK/Inc/DDSTextureLoader.h>

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
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(270, 360));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                if (ImGui::BeginTabItem("OG"))
                {
                    render_og_textures();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Remastered"))
                {
                    render_remastered_textures();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        // ImGui::PopStyleVar();
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

    void TexturesWindow::render_og_textures()
    {
        ImGui::Checkbox(Names::transparency_checkbox.c_str(), &_transparency);

        if (ImGui::InputInt(Names::tile.c_str(), &_index))
        {
            clamp_index();
        }

        if (_texture_storage && _index < static_cast<int32_t>(_texture_storage->num_tiles()))
        {
            auto texture = _transparency ? _texture_storage->texture(_index) : _texture_storage->opaque_texture(_index);
            ImGui::Image(texture.view().Get(), ImVec2(512, 512));
        }
    }

    void TexturesWindow::render_remastered_textures()
    {
        if (ImGui::InputInt(Names::tile.c_str(), &_remastered_index))
        {
            clamp_remastered_index();
        }

        if (_texture_storage && _remastered_index < static_cast<int32_t>(_texture_storage->num_remastered_textures()))
        {
            auto texture = _texture_storage->remastered_texture(_remastered_index);
            ImGui::Text(texture.name().c_str());
            ImGui::Image(texture.view().Get(), ImVec2(512, 512));
        }
    }

    void TexturesWindow::clamp_remastered_index()
    {
        const int32_t max_textures = std::max(0, (_texture_storage ? static_cast<int32_t>(_texture_storage->num_remastered_textures()) : 0) - 1);
        _remastered_index = std::clamp(_remastered_index, 0, max_textures);
    }
}
