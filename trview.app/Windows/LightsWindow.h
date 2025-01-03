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
        virtual void clear_selected_light() override;
        virtual void render() override;
        virtual void update(float delta) override;
        virtual void set_lights(const std::vector<std::weak_ptr<ILight>>& lights) override;
        virtual void set_selected_light(const std::weak_ptr<ILight>& light) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_number(int32_t number) override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
    private:
        void set_sync_light(bool value);
        void set_local_selected_light(const std::weak_ptr<ILight>& light);
        void render_lights_list();
        void render_light_details();
        bool render_lights_window();
        void setup_filters();
        void calculate_column_widths();

        std::vector<std::weak_ptr<ILight>> _all_lights;
        std::shared_ptr<IClipboard> _clipboard;
        bool _sync_light{ true };
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Tomb5 };
        std::optional<float> _tooltip_timer;
        std::string _id{ "Lights 0" };
        bool _scroll_to_light{ false };
        std::weak_ptr<ILight> _selected_light;
        std::weak_ptr<ILight> _global_selected_light;
        std::weak_ptr<IRoom> _current_room;
        std::unordered_map<std::string, std::string> _tips;
        Filters<ILight> _filters;
        bool _force_sort{ false };
        Track<Type::Room> _track;
        ColumnSizer _column_sizer;
        AutoHider _auto_hider;
    };
}
