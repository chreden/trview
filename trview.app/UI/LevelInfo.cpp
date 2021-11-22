#include "LevelInfo.h"

#include <trview.ui/Control.h>
#include <trview.ui/Image.h>
#include <trview.ui/Label.h>
#include <trview.ui/Button.h>
#include <trview.app/Graphics/ITextureStorage.h>
#include <trview.common/Strings.h>
#include "../Resources/resource.h"

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

    const std::string LevelInfo::Names::version{ "version" };
    const std::string LevelInfo::Names::name{ "name" };
    const std::string LevelInfo::Names::settings{ "settings" };

    LevelInfo::LevelInfo(ui::Control& control, const ITextureStorage& texture_storage, const ui::UiSource& ui_source)
    {
        for (const auto& tex : texture_names)
        {
            _version_textures.insert({ tex.first, texture_storage.lookup(tex.second) });
        }

        using namespace ui;
        auto panel = control.add_child(ui_source(IDR_UI_LEVEL_INFO));
        _version = panel->find<Image>(Names::version);
        _version->set_texture(get_version_image(trlevel::LevelVersion::Unknown));

        _name = panel->find<Label>(Names::name);

        auto settings = panel->find<Button>(Names::settings);
        settings->set_images(texture_storage.lookup("settings"), texture_storage.lookup("settings"));
        settings->on_click += on_toggle_settings;

        // Have the control move itself when the parent control resizes.
        _token_store += control.on_size_changed += [=](const Size& size)
        {
            panel->set_position(Point(size.width / 2.0f - panel->size().width / 2.0f, 0));
        };
        _token_store += panel->on_size_changed += [=](const Size& size)
        {
            auto parent = panel->parent();
            panel->set_position(Point(parent->size().width / 2.0f - size.width / 2.0f, 0));
        };
    }

    // Set the name of the level.
    // name: The level name.
    void LevelInfo::set_level(const std::string& name)
    {
        _name->set_text(to_utf16(name));
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