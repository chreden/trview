#pragma once

#include <trview.common/Windows/IClipboard.h>
#include "../Elements/ILight.h"
#include "ILightsWindow.h"
#include "../Filters/Filters.h"
#include "../Track/Track.h"
#include "ColumnSizer.h"
#include "AutoHider.h"

namespace trview
{
    class LightsWindow final : public ILightsWindow
    {
    public:
        struct Names
        {
            static inline const std::string light_list_panel = "Light List";
            static inline const std::string lights_listbox = "Lights";
            static inline const std::string stats_listbox = "Stats";
            static inline const std::string sync_light = "Sync";
            static inline const std::string track_room = "Track Room";
            static inline const std::string details_panel = "Light Details";
        };

        explicit LightsWindow(const std::shared_ptr<IClipboard>& clipboard);
        virtual ~LightsWindow() = default;
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void render() override;
        void set_selected_light(const std::weak_ptr<ILight>& light) override;
        void set_number(int32_t number) override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
        void update(float delta) override;
    private:
        bool render_lights_window();

        struct SubWindow
        {
            Event<std::weak_ptr<ILight>> on_light_selected;
            Event<> on_scene_changed;

            std::vector<std::weak_ptr<ILight>> _all_lights;
            AutoHider _auto_hider;
            std::shared_ptr<IClipboard> _clipboard;
            ColumnSizer _column_sizer;
            std::weak_ptr<IRoom> _current_room;
            Filters<ILight> _filters;
            bool _force_sort{ false };
            std::weak_ptr<ILight> _global_selected_light;
            std::weak_ptr<ILevel> _level;
            trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Tomb5 };
            bool _scroll_to_light{ false };
            std::weak_ptr<ILight> _selected_light;
            bool _sync_light{ true };
            std::unordered_map<std::string, std::string> _tips;
            std::optional<float> _tooltip_timer;
            Track<Type::Room> _track;

            void calculate_column_widths();
            void render(int index);
            void render_lights_list();
            void render_light_details();
            void set_current_room(const std::weak_ptr<IRoom>& room);
            void set_sync_light(bool value);
            void set_lights(const std::vector<std::weak_ptr<ILight>>& lights);
            void set_local_selected_light(const std::weak_ptr<ILight>& light);
            void set_selected_light(const std::weak_ptr<ILight>& light);
            void setup_filters();
            void update(float delta);
        };

        std::string _id{ "Lights 0" };
        std::vector<SubWindow> _sub_windows;
        std::shared_ptr<IClipboard> _clipboard;
    };
}
