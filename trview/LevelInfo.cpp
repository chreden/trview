#include "stdafx.h"
#include "LevelInfo.h"

#include <trview.ui/Control.h>
#include <trview.ui/Image.h>
#include <trview.ui/Label.h>
#include <trview.ui/StackPanel.h>
#include "ITextureStorage.h"

namespace trview
{
    namespace
    {
        // Ideally this will be stored somewhere else.
        const std::unordered_map<trlevel::LevelVersion, std::string> texture_names
        {
            { trlevel::LevelVersion::Unknown, "unknown" },
            { trlevel::LevelVersion::Tomb1, "tomb1" },
            { trlevel::LevelVersion::Tomb2, "tomb2" },
            { trlevel::LevelVersion::Tomb3, "tomb3" },
            { trlevel::LevelVersion::Tomb4, "tomb4" },
            { trlevel::LevelVersion::Tomb5, "tomb5" },
        };
    }

    LevelInfo::LevelInfo(ui::Control& control, const ITextureStorage& texture_storage)
    {
        for (const auto& tex : texture_names)
        {
            _version_textures.insert({ tex.first, texture_storage.lookup(tex.second) });
        }

        using namespace ui;
        auto panel = std::make_unique<StackPanel>(Point(500, 0), Size(100, 24), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(5, 5), StackPanel::Direction::Horizontal);
        auto version = std::make_unique<Image>(Point(), Size(16, 16));
        version->set_texture(get_version_image(trlevel::LevelVersion::Unknown));

        auto name = std::make_unique<Label>(Point(), Size(74, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"No level", 10.0f, TextAlignment::Centre, ParagraphAlignment::Centre);

        _version = version.get();
        _name = name.get();

        panel->add_child(std::move(version));
        panel->add_child(std::move(name));
        control.add_child(std::move(panel));
    }

    void LevelInfo::set_level(const std::wstring& name)
    {
        _name->set_text(name);
    }

    void LevelInfo::set_level_version(trlevel::LevelVersion version)
    {
        _version->set_texture(get_version_image(version));
    }

    Texture LevelInfo::get_version_image(trlevel::LevelVersion version) const
    {
        auto found = _version_textures.find(version);
        if (found == _version_textures.end())
        {
            return Texture();
        }
        return found->second;
    }
}