#pragma once

#include "ISoundsWindow.h"
#include "../ColumnSizer.h"
#include "../../Filters/Filters.h"
#include "../AutoHider.h"
#include "../../Elements/ILevel.h"

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
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void render() override;
        void set_number(int32_t number) override;
        void set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source) override;
    private:
        bool render_sounds_window();

        struct SubWindow
        {
            Event<> on_scene_changed;
            Event<std::weak_ptr<ISoundSource>> on_sound_source_selected;

            AutoHider _auto_hider;
            std::vector<std::weak_ptr<ISoundSource>> _all_sound_sources;
            ColumnSizer _column_sizer;
            Filters<ISoundSource> _filters;
            bool _force_sort = false;
            std::weak_ptr<ISoundSource> _global_selected_sound_source;
            std::weak_ptr<ILevel> _level;
            trlevel::Platform _level_platform{ trlevel::Platform::Unknown };
            trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
            bool _scroll_to_sound_source{ false };
            std::weak_ptr<ISoundSource> _selected_sound_source;
            std::weak_ptr<ISoundStorage> _sound_storage;
            bool _sync_sound_source{ true };
            

            void calculate_column_widths();
            void render(int index);
            void render_sound_sources_list();
            void render_sounds_source_details();
            void render_sound_board();
            void set_local_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source);
            void set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source);
            void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage);
            void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources);
            void set_sync_sound_source(bool value);
            void setup_filters();
        };

        std::string _id{ "Sounds 0" };
        std::vector<SubWindow> _sub_windows;
    };
}
