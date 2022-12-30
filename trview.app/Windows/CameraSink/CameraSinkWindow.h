#pragma once

#include <trview.common/Windows/IClipboard.h>
#include "../../Filters/Filters.h"
#include "ICameraSinkWindow.h"

namespace trview
{
    class CameraSinkWindow final : public ICameraSinkWindow
    {
    public:
        struct Names
        {
            static inline const std::string sync = "Sync";
            static inline const std::string track_room = "Track Room";
            static inline const std::string camera_sink_list = "##camerasinklist";
            static inline const std::string list_panel = "Camera/Sink List";
            static inline const std::string details_panel = "Camera/Sink Details";
            static inline const std::string stats_listbox = "Stats";
            static inline const std::string camera_listbox = "Camera";
            static inline const std::string sink_listbox = "Sink";
            static inline const std::string triggers_list = "##triggeredby";
        };

        explicit CameraSinkWindow(const std::shared_ptr<IClipboard>& clipboard);
        virtual ~CameraSinkWindow() = default;
        virtual void render() override;
        virtual void set_number(int32_t number) override;
        virtual void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) override;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
    private:
        bool render_camera_sink_window();
        void set_track_room(bool value);
        void set_sync(bool value);
        void set_local_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink);
        void render_list();
        void render_details();

        std::string _id{ "Camera/Sink 0" };
        std::vector<std::weak_ptr<ICameraSink>> _all_camera_sinks;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        std::weak_ptr<ICameraSink> _global_selected_camera_sink;
        std::unordered_map<std::string, std::string> _tips;
        std::shared_ptr<IClipboard> _clipboard;
        Filters<ICameraSink> _filters;
        std::weak_ptr<ITrigger> _selected_trigger;
        bool _track_room{ false };
        bool _sync{ true };
        bool _scroll_to{ false };
        std::optional<float> _tooltip_timer;
        uint32_t _current_room{ 0u };
        bool _force_sort{ false };
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        std::vector<std::weak_ptr<ITrigger>> _triggered_by;
    };
}
