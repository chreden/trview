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
        void add_level(const std::weak_ptr<ILevel>& level) override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        std::weak_ptr<ICameraSinkWindow> create_window() override;
        void render() override;
        void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
    private:
        struct Selection
        {
            std::weak_ptr<ILevel> level;
            std::weak_ptr<IRoom> room;
            std::weak_ptr<ICameraSink> camera_sink;
        };
        std::vector<Selection> _levels;
        ICameraSinkWindow::Source _camera_sink_window_source;
    };
}
