/// @file LevelInfo.h
/// @brief UI element that shows the name of the currently loaded level as well as which game it is made for.
/// 
/// Adds a small window to the top of the screen that show the icon of the game that the level is made for 
/// as well as the final part of the level filename.

#pragma once

#include <string>
#include <unordered_map>
#include <trlevel/LevelVersion.h>
#include <trview.graphics/Texture.h>
#include <trview.common/Event.h>
#include <trview.app/Graphics/ITextureStorage.h>

namespace trview
{
    /// The level info display shows the name of the current level as well
    /// as the game that the level was built for.
    class LevelInfo
    {
    public:
        /// Creates an instance of the LevelInfo class. 
        /// @param texture_storage Texture storage instance to use.
        explicit LevelInfo(const ITextureStorage& texture_storage);

        void render();

        /// Sets the name of the level.
        /// @param name The level name.
        void set_level(const std::string& name);

        /// Set the version of the game that level was created for.
        /// @param version The version of the game.
        /// @see trlevel::LevelVersion.
        void set_level_version(trlevel::LevelVersion version);

        /// Event raised when the settings button is pressed.
        Event<> on_toggle_settings;
    private:
        graphics::Texture get_version_image(trlevel::LevelVersion version) const;
        std::unordered_map<trlevel::LevelVersion, graphics::Texture> _version_textures;
        std::string _name{ "No Level" };
        trlevel::LevelVersion _version{ trlevel::LevelVersion::Unknown };
    };
}
