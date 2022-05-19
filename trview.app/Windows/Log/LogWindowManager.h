#pragma once

#include "../WindowManager.h"
#include "ILogWindowManager.h"
#include "ILogWindow.h"

namespace trview
{
    class LogWindowManager final : public ILogWindowManager, WindowManager<ILogWindow>, public MessageHandler
    {
    public:
        LogWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILogWindow::Source& log_window_source);
        virtual ~LogWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual std::weak_ptr<ILogWindow> create_window() override;
        virtual void render() override;
    private:
        ILogWindow::Source _log_window_source;
    };
}
