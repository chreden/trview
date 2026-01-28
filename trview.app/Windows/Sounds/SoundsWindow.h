#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Messages/IMessageSystem.h>

#include "../../Filters/Filters.h"
#include "../AutoHider.h"
#include "../../Settings/UserSettings.h"

#include <trlevel/LevelVersion.h>
#include "../../Elements/SoundSource/ISoundSource.h"
#include "../../Sound/ISoundStorage.h"

#include "../IWindow.h"

namespace trview
{
    struct ILevel;

    class SoundsWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string details_listbox = "Details Stats";
            static inline const std::string details_panel = "Sound Source Details";
            static inline const std::string sound_sources_list = "##soundsourceslist";
            static inline const std::string sound_source_list_panel = "Sound Source List";
            static inline const std::string sound_source_stats = "##soundsourcestats";
            static inline const std::string stats_listbox = "Source Stats";
            static inline const std::string sync_sound_source = "Sync";
        };

        explicit SoundsWindow(const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~SoundsWindow() = default;
        void update(float delta) override;
        void render() override;
        void set_level_platform(trlevel::Platform platform);
        void set_level_version(trlevel::LevelVersion version);
        void set_number(int32_t number) override;
        void set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source);
        void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage);
        void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources);
        void receive_message(const Message& message) override;
        void initialise();
        std::string type() const override;
        std::string title() const override;
    private:
        bool render_sounds_window();
        void render_sound_sources_list();
        void render_sounds_source_details();
        void render_sound_board();
        void set_local_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source);
        void set_sync_sound_source(bool value);
        void setup_filters();

        std::string _id{ "Sounds 0" };
        bool _force_sort = false;
        bool _sync_sound_source{ true };
        std::vector<std::weak_ptr<ISoundSource>> _all_sound_sources;
        std::weak_ptr<ISoundStorage> _sound_storage;
        std::weak_ptr<ISoundSource> _selected_sound_source;
        std::weak_ptr<ISoundSource> _global_selected_sound_source;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        Filters _filters;
        trlevel::Platform _level_platform{ trlevel::Platform::Unknown };
        TokenStore _token_store;
        AutoHider _auto_hider;
        bool _columns_set{ false };
        std::optional<UserSettings> _settings;
        std::weak_ptr<IMessageSystem> _messaging;
        std::weak_ptr<ILevel> _level;
    };
}
