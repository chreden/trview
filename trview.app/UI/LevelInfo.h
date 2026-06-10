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
#include "../Elements/Level/ILevelNameLookup.h"
#include "../Graphics/ITextureStorage.h"
#include "ILevelInfo.h"
#include <trview.common/Messages/IRecipient.h>

namespace trview
{
    struct ILevel;

    /// The level info display shows the name of the current level as well
    /// as the game that the level was built for.
    class LevelInfo final : public ILevelInfo, public IRecipient
    {
    public:
        /// Creates an instance of the LevelInfo class. 
        /// @param texture_storage Texture storage instance to use.
        explicit LevelInfo(const ITextureStorage& texture_storage, const std::shared_ptr<ILevelNameLookup>& level_name_lookup);
        virtual ~LevelInfo() = default;
        void render() override;
        void receive_message(const Message& message) override;
    private:
        void set_level(const std::weak_ptr<ILevel>& level);

        graphics::Texture get_version_image(trlevel::LevelVersion version) const;
        std::unordered_map<trlevel::LevelVersion, graphics::Texture> _version_textures;
        std::string _name{ "No Level" };
        trlevel::LevelVersion _version{ trlevel::LevelVersion::Unknown };
        std::shared_ptr<ILevelNameLookup> _level_name_lookup;
    };
}
