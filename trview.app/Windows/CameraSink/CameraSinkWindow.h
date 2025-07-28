#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Windows/IClipboard.h>
#include "../../Filters/Filters.h"
#include "../../Track/Track.h"
#include "ICameraSinkWindow.h"
#include "../AutoHider.h"
#include "../../Settings/UserSettings.h"

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

        explicit CameraSinkWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<ICamera>& camera);
        virtual ~CameraSinkWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) override;
        void set_flybys(const std::vector<std::weak_ptr<IFlyby>>& flybys) override;
        void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        void set_selected_flyby_node(const std::weak_ptr<IFlybyNode>& flyby_node) override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
        void set_settings(const UserSettings& settings) override;
        void update(float delta) override;
        void set_platform_and_version(const trlevel::PlatformAndVersion& version) override;
    private:
        bool render_camera_sink_window();
        void set_sync(bool value);
        void set_local_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink);
        void render_list();
        void render_details();
        void setup_filters();
        void setup_flyby_filters();
        void render_flybys();
        void sync_flyby();
        void render_flyby_list();
        void render_flyby_tab();
        void render_flyby_details();
        void set_local_selected_flyby_node(const std::weak_ptr<IFlybyNode>& flyby_node);

        template <typename T>
        void add_stat(const std::string& name, const T&& value);

        void capture_camera_state();
        void restore_camera_state();

        std::string _id{ "Camera/Sink 0" };
        // Camera sinks
        std::vector<std::weak_ptr<ICameraSink>> _all_camera_sinks;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        std::weak_ptr<ICameraSink> _global_selected_camera_sink;
        Filters<ICameraSink> _filters;

        // Flybys
        std::vector<std::weak_ptr<IFlyby>> _all_flybys;
        std::weak_ptr<IFlyby> _selected_flyby;
        std::weak_ptr<IFlybyNode> _global_selected_flyby_node;
        Filters<IFlyby> _flyby_filters;
        Filters<IFlybyNode> _node_filters;
        std::weak_ptr<IFlybyNode> _selected_node;

        std::unordered_map<std::string, std::string> _tips;
        std::shared_ptr<IClipboard> _clipboard;

        std::weak_ptr<ITrigger> _selected_trigger;
        bool _sync{ true };
        bool _scroll_to{ false };
        std::weak_ptr<IRoom> _current_room;
        std::vector<std::weak_ptr<ITrigger>> _triggered_by;
        Track<Type::Room> _track;
        AutoHider _auto_hider;
        std::weak_ptr<ICamera> _camera;

        bool _playing_flyby{ false };
        UserSettings _settings;
        bool _columns_set{ false };
        bool _force_sort{ false };
        TokenStore _token_store;
        IFlyby::CameraState _state;
        std::optional<IFlyby::CameraState> _initial_state;
        trlevel::PlatformAndVersion _platform_and_version;
    };
}

#include "CameraSinkWindow.inl"

