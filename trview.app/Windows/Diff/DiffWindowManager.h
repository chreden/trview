#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/Shortcuts.h>
#include "../WindowManager.h"
#include "IDiffWindowManager.h"
#include "IDiffWindow.h"

namespace trview
{
    class DiffWindowManager final : public IDiffWindowManager, public WindowManager<IDiffWindow>, public MessageHandler
    {
    public:
        DiffWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IDiffWindow::Source& diff_window_source);
        virtual ~DiffWindowManager() = default;
        std::weak_ptr<IDiffWindow> create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
    private:
        IDiffWindow::Source _diff_window_source;
        std::weak_ptr<ILevel> _level;
    };
}
