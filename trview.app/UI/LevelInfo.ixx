export module trview.app:LevelInfo;

/// @file LevelInfo.h
/// @brief UI element that shows the name of the currently loaded level as well as which game it is made for.
/// 
/// Adds a small window to the top of the screen that show the icon of the game that the level is made for 
/// as well as the final part of the level filename.

import std;

import trlevel;
import trview.graphics;

import :ILevelInfo;
import :ILevelNameLookup;
import :ITextureStorage;

namespace trview
{
    struct ILevel;

    /// The level info display shows the name of the current level as well
    /// as the game that the level was built for.
    export class LevelInfo final : public ILevelInfo
    {
    public:
        /// Creates an instance of the LevelInfo class. 
        /// @param texture_storage Texture storage instance to use.
        explicit LevelInfo(const ITextureStorage& texture_storage, const std::shared_ptr<ILevelNameLookup>& level_name_lookup);
        virtual ~LevelInfo() = default;
        void render() override;
        /// Sets the name of the level.
        /// @param name The level name.
        void set_level(const std::weak_ptr<ILevel>& level) override;
    private:
        graphics::Texture get_version_image(trlevel::LevelVersion version) const;
        std::unordered_map<trlevel::LevelVersion, graphics::Texture> _version_textures;
        std::string _name{ "No Level" };
        trlevel::LevelVersion _version{ trlevel::LevelVersion::Unknown };
        std::shared_ptr<ILevelNameLookup> _level_name_lookup;
    };
}
