#include "LevelInfo.h"
#include <trview.app/Graphics/ITextureStorage.h>

namespace trview
{
    namespace
    {
        const std::unordered_map<trlevel::LevelVersion, std::string> texture_names
        {
            { trlevel::LevelVersion::Unknown, "unknown_icon" },
            { trlevel::LevelVersion::Tomb1, "tomb1_icon" },
            { trlevel::LevelVersion::Tomb2, "tomb2_icon" },
            { trlevel::LevelVersion::Tomb3, "tomb3_icon" },
            { trlevel::LevelVersion::Tomb4, "tomb4_icon" },
            { trlevel::LevelVersion::Tomb5, "tomb5_icon" },
        };
    }

    LevelInfo::LevelInfo(const ITextureStorage& texture_storage)
    {
        for (const auto& tex : texture_names)
        {
            _version_textures.insert({ tex.first, texture_storage.lookup(tex.second) });
        }
    }

    void LevelInfo::render()
    {
        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y), 0, ImVec2(0.5f, 0.0f));

        if (ImGui::Begin("LevelInfo", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Image(get_version_image(_version).view().Get(), ImVec2(16, 16));
            ImGui::SameLine();
            const float previous = ImGui::GetCursorPosY();
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.5f - ImGui::CalcTextSize(_name.c_str()).y * 0.5f);
            ImGui::Text(_name.c_str());
            ImGui::SameLine();
            ImGui::SetCursorPosY(previous);
            if (ImGui::Button("Settings"))
            {
                on_toggle_settings();
            }
        }
        ImGui::End();
    }

    void LevelInfo::set_level(const std::string& name)
    {
        _name = name;
    }

    void LevelInfo::set_level_version(trlevel::LevelVersion version)
    {
        _version = version;
    }

    graphics::Texture LevelInfo::get_version_image(trlevel::LevelVersion version) const
    {
        auto found = _version_textures.find(version);
        if (found == _version_textures.end())
        {
            return graphics::Texture();
        }
        return found->second;
    }
}