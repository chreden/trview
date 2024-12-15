#pragma once

#include "ISoundsWindow.h"
#include "../ColumnSizer.h"
#include "../../Filters/Filters.h"
#include "../AutoHider.h"

namespace trview
{
    struct ILevel;

    class SoundsWindow final : public ISoundsWindow
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

        explicit SoundsWindow();
        virtual ~SoundsWindow() = default;
        void render() override;
        void set_level_platform(trlevel::Platform platform) override;
        void set_level_version(trlevel::LevelVersion version) override;
        void set_number(int32_t number) override;
        void set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source) override;
        void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage) override;
        void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources) override;
    private:
        bool render_sounds_window();
        void calculate_column_widths();
        void render_sound_sources_list();
        void render_sounds_source_details();
        void render_sound_board();
        void set_local_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source);
        void set_sync_sound_source(bool value);
        void setup_filters();

        std::string _id{ "Sounds 0" };
        bool _force_sort = false;
        bool _scroll_to_sound_source{ false };
        bool _sync_sound_source{ true };
        std::vector<std::weak_ptr<ISoundSource>> _all_sound_sources;
        ColumnSizer _column_sizer;
        std::weak_ptr<ISoundStorage> _sound_storage;
        std::weak_ptr<ISoundSource> _selected_sound_source;
        std::weak_ptr<ISoundSource> _global_selected_sound_source;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        Filters<ISoundSource> _filters;
        trlevel::Platform _level_platform{ trlevel::Platform::Unknown };
        AutoHider _auto_hider;
    };
}
