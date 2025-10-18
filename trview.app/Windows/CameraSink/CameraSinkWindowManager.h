#pragma once

#include <trview.common/MessageHandler.h>
#include "ICameraSinkWindowManager.h"
#include "../WindowManager.h"

namespace trview
{
    class CameraSinkWindowManager final : public ICameraSinkWindowManager, public WindowManager<ICameraSinkWindow>, public MessageHandler
    {
    public:
        CameraSinkWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ICameraSinkWindow::Source& camera_sink_window_source);
        virtual ~CameraSinkWindowManager() = default;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        std::weak_ptr<ICameraSinkWindow> create_window() override;
        void render() override;
        void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) override;
        void set_flybys(const std::vector<std::weak_ptr<IFlyby>>& flybys) override;
        void set_platform_and_version(const trlevel::PlatformAndVersion& platform_and_version) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        void update(float delta) override;
    private:
        ICameraSinkWindow::Source _camera_sink_window_source;
        std::vector<std::weak_ptr<ICameraSink>> _camera_sinks;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        std::weak_ptr<IFlybyNode> _selected_flyby_node;
        std::weak_ptr<IRoom> _current_room;
        std::vector<std::weak_ptr<IFlyby>> _flybys;
        trlevel::PlatformAndVersion _platform_and_version;
    };
}
