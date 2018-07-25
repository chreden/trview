#include "stdafx.h"
#include "LevelInfo.h"

#include <trview.ui/Control.h>
#include <trview.ui/Image.h>
#include <trview.ui/Label.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/Button.h>
#include "ITextureStorage.h"

namespace trview
{
    namespace
    {
        // Ideally this will be stored somewhere else.
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

    LevelInfo::LevelInfo(ui::Control& control, const ITextureStorage& texture_storage)
    {
        for (const auto& tex : texture_names)
        {
            _version_textures.insert({ tex.first, texture_storage.lookup(tex.second) });
        }

        using namespace ui;
        auto panel = std::make_unique<StackPanel>(Point(control.size().width / 2.0f - 50, 0), Size(100, 24), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(5, 5), StackPanel::Direction::Horizontal, SizeMode::Auto);
        auto version = std::make_unique<Image>(Point(), Size(16, 16));
        version->set_background_colour(Colour(0, 0, 0, 0));
        version->set_texture(get_version_image(trlevel::LevelVersion::Unknown));

        auto name = std::make_unique<Label>(Point(), Size(74, 16), Colour(1.0f, 0.5f, 0.5f, 0.5f), L"No level", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre, SizeMode::Auto);
        name->set_vertical_alignment(Align::Centre);

        auto settings = std::make_unique<Button>(Point(), Size(16, 16), texture_storage.lookup("settings"), texture_storage.lookup("settings"));
        settings->on_click += on_toggle_settings;

        _panel = panel.get();
        _version = version.get();
        _name = name.get();

        panel->add_child(std::move(version));
        panel->add_child(std::move(name));
        panel->add_child(std::move(settings));
        control.add_child(std::move(panel));

        // Have the control move itself when the parent control resizes.
        control.on_size_changed += [&](const Size& size)
        {
            _panel->set_position(Point(size.width / 2.0f - _panel->size().width / 2.0f, 0));
        };
        _panel->on_size_changed += [&](const Size& size)
        {
            auto parent = _panel->parent();
            _panel->set_position(Point(parent->size().width / 2.0f - size.width / 2.0f, 0));
        };
    }

    // Set the name of the level.
    // name: The level name.
    void LevelInfo::set_level(const std::wstring& name)
    {
        _name->set_text(name);
    }

    // Set the version of the game that level was created for.
    // version: The version of the game.
    void LevelInfo::set_level_version(trlevel::LevelVersion version)
    {
        _version->set_texture(get_version_image(version));
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