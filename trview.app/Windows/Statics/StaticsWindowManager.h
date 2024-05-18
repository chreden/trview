#pragma once

#include <minwindef.h>
#include <memory>
#include <optional>

#include <trview.common/Window.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/IShortcuts.h>

#include "../WindowManager.h"
#include "IStaticsWindowManager.h"
#include "IStaticsWindow.h"

namespace trview
{
    class StaticsWindowManager final : public IStaticsWindowManager, public WindowManager<IStaticsWindow>, public MessageHandler
    {
    public:
        explicit StaticsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IStaticsWindow::Source& statics_window_source);
        virtual ~StaticsWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        virtual std::weak_ptr<IStaticsWindow> create_window() override;
        virtual void update(float delta) override;
    private:
        IStaticsWindow::Source _source;
    };
}
