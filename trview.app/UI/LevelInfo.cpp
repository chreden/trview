#include "LevelInfo.h"
#include <trview.app/Graphics/ITextureStorage.h>
#include "../Elements/ILevel.h"
#include "../Messages/Messages.h"

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

    ILevelInfo::~ILevelInfo()
    {
    }

    LevelInfo::LevelInfo(const ITextureStorage& texture_storage, const std::shared_ptr<ILevelNameLookup>& level_name_lookup)
        : _level_name_lookup(level_name_lookup)
    {
        for (const auto& tex : texture_names)
        {
            _version_textures.insert({ tex.first, texture_storage.lookup(tex.second) });
        }
    }

    void LevelInfo::render()
    {
        const ImVec2 full_text_size = ImGui::CalcTextSize(_name.c_str());
        const float text_size = full_text_size.y;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - full_text_size.x * 0.5f - text_size);
        const float previous = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.5f - text_size * 0.5f);
        ImGui::Image(ImTextureID(get_version_image(_version).view().Get()), ImVec2(text_size, text_size));
        ImGui::SetCursorPosY(previous);
        ImGui::Text(_name.c_str());
    }

    void LevelInfo::set_level(const std::weak_ptr<ILevel>& level)
    {
        if (const auto level_ptr = level.lock())
        {
            if (auto name = _level_name_lookup->lookup(level))
            {
                _name = std::format("{} ({})", name.value().name, level_ptr->name());
            }
            else
            {
                _name = level_ptr->name();
            }
            _version = level_ptr->version();
        }
        else
        {
            _name.clear();
            _version = trlevel::LevelVersion::Unknown;
        }
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

    void LevelInfo::receive_message(const Message& message)
    {
        if (const auto level = messages::read_open_level(message))
        {
            set_level(level.value());
        }
    }
}