#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/Shortcuts.h>
#include "../WindowManager.h"
#include "IDiffWindowManager.h"
#include "IDiffWindow.h"

namespace trview
{
    class DiffWindowManager final : public IDiffWindowManager, WindowManager<IDiffWindow>, public MessageHandler
    {
    public:
        DiffWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IDiffWindow::Source& diff_window_source);
        virtual ~DiffWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual std::weak_ptr<IDiffWindow> create_window() override;
        virtual void render() override;
    private:
        IDiffWindow::Source _diff_window_source;
    };
}
