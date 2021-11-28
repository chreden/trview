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
#include <trview.common/TokenStore.h>
#include <trview.app/Graphics/ITextureStorage.h>
#include <trview.ui/Control.h>

namespace trview
{
    namespace ui
    {
        class Label;
        class Image;
    }

    /// The level info display shows the name of the current level as well
    /// as the game that the level was built for.
    class LevelInfo
    {
    public:
        struct Names
        {
            static const std::string version;
            static const std::string name;
            static const std::string settings;
        };

        /// Creates an instance of the LevelInfo class. This will add UI elements to the 
        /// control provided.
        /// @param parent The control to which the instance will be added as a child.
        /// @param texture_storage Texture storage instance to use.
        LevelInfo(ui::Control& parent, const ITextureStorage& texture_storage, const ui::UiSource& ui_source);

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

        ui::Label* _name;
        ui::Image* _version;
        std::unordered_map<trlevel::LevelVersion, graphics::Texture> _version_textures;
        TokenStore _token_store;
    };
}
