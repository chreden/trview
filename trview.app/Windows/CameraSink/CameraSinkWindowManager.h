#pragma once

#include <trview.common/MessageHandler.h>
#include "ICameraSinkWindowManager.h"
#include "../WindowManager.h"

namespace trview
{
    class CameraSinkWindowManager final : public ICameraSinkWindowManager, public WindowManager<ICameraSinkWindow>, public MessageHandler
    {
    public:
        CameraSinkWindowManager(const Window& window, const ICameraSinkWindow::Source& camera_sink_window_source);
        virtual ~CameraSinkWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual std::weak_ptr<ICameraSinkWindow> create_window() override;
        virtual void render() override;
        virtual void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) override;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        virtual void set_room(uint32_t room) override;
    private:
        ICameraSinkWindow::Source _camera_sink_window_source;
        std::vector<std::weak_ptr<ICameraSink>> _camera_sinks;
        std::weak_ptr<ICameraSink> _selected_camera_sink;
        uint32_t _current_room{ 0u };
    };
}
