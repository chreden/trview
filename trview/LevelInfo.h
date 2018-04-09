#pragma once

#include <string>
#include <unordered_map>

#include <trlevel/LevelVersion.h>
#include <trview.common/Texture.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Label;
        class Image;
    }

    struct ITextureStorage;

    // The level info display shows the name of the current level as well
    // as the game that the level was built for.
    class LevelInfo
    {
    public:
        LevelInfo(ui::Control& parent, const ITextureStorage& texture_storage);

        // Set the name of the level.
        // name: The level name.
        void set_level(const std::wstring& name);

        // Set the version of the game that level was created for.
        // version: The version of the game.
        void set_level_version(trlevel::LevelVersion version);
    private:
        Texture get_version_image(trlevel::LevelVersion version) const;

        ui::Label* _name;
        ui::Image* _version;
        std::unordered_map<trlevel::LevelVersion, Texture> _version_textures;
    };
}
