#pragma once

#include "ISoundsWindow.h"
#include "../ColumnSizer.h"

namespace trview
{
    class SoundsWindow final : public ISoundsWindow
    {
    public:
        struct Names
        {
            static inline const std::string sound_sources_list = "##soundsourceslist";
            static inline const std::string sound_source_list_panel = "Sound Source List";
            static inline const std::string details_panel = "Sound Source Details";
            static inline const std::string sound_source_stats = "##itemstats";
            static inline const std::string stats_listbox = "Stats";
        };

        virtual ~SoundsWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage) override;
        void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources) override;
    private:
        bool render_sounds_window();
        void calculate_column_widths();
        void render_sound_sources_list();
        void render_sounds_source_details();
        void render_sound_board();
        void set_local_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source);

        std::string _id{ "Sounds 0" };
        bool _force_sort = false;
        std::vector<std::weak_ptr<ISoundSource>> _all_sound_sources;
        ColumnSizer _column_sizer;
        std::weak_ptr<ISoundStorage> _sound_storage;
        std::weak_ptr<ISoundSource> _selected_sound_source;
    };
}
