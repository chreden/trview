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
        void update(float delta) override;
    private:
        ICameraSinkWindow::Source _camera_sink_window_source;
    };
}
