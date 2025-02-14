#pragma once

#include <trview.common/Windows/IClipboard.h>
#include "../../Filters/Filters.h"
#include "../../Track/Track.h"
#include "ICameraSinkWindow.h"
#include "../ColumnSizer.h"
#include "../AutoHider.h"

namespace trview
{
    class CameraSinkWindow final : public ICameraSinkWindow
    {
    public:
        struct Names
        {
            static inline const std::string sync = "Sync";
            static inline const std::string camera_sink_list = "##camerasinklist";
            static inline const std::string list_panel = "Camera/Sink List";
            static inline const std::string details_panel = "Details";
            static inline const std::string stats_listbox = "Stats";
            static inline const std::string triggers_list = "##triggeredby";
            static inline const std::string type = "Type";
        };

        explicit CameraSinkWindow(const std::shared_ptr<IClipboard>& clipboard);
        virtual ~CameraSinkWindow() = default;
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void render() override;
        void set_number(int32_t number) override;
        void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
    private:
        bool render_camera_sink_window();

        struct SubWindow
        {
            Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
            Event<std::weak_ptr<ITrigger>> on_trigger_selected;
            Event<> on_scene_changed;

            std::vector<std::weak_ptr<ICameraSink>> _all_camera_sinks;
            AutoHider _auto_hider;
            std::shared_ptr<IClipboard> _clipboard;
            ColumnSizer _column_sizer;
            std::weak_ptr<IRoom> _current_room;
            Filters<ICameraSink> _filters;
            bool _force_sort{ false };
            std::weak_ptr<ICameraSink> _global_selected_camera_sink;
            std::weak_ptr<ILevel> _level;
            std::weak_ptr<ICameraSink> _selected_camera_sink;
            std::weak_ptr<ITrigger> _selected_trigger;
            bool _scroll_to{ false };
            bool _sync{ true };
            std::unordered_map<std::string, std::string> _tips;
            Track<Type::Room> _track;
            std::vector<std::weak_ptr<ITrigger>> _triggered_by;

            void calculate_column_widths();
            void render(int index);
            void render_details();
            void render_list();
            void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks);
            void set_current_room(const std::weak_ptr<IRoom>& room);
            void set_local_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink);
            void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink);
            void set_sync(bool value);
            void setup_filters();
        };

        std::string _id{ "Camera/Sink 0" };
        std::vector<SubWindow> _sub_windows;
        std::shared_ptr<IClipboard> _clipboard;
    };
}
