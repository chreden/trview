#pragma once

#include "ILauWindowManager.h"
#include "ILauWindow.h"
#include <trview.common/TokenStore.h>
#include <trview.common/MessageHandler.h>

namespace trview
{
    class LauWindowManager final : public ILauWindowManager, public MessageHandler
    {
    public:
        LauWindowManager(const Window& window, const ILauWindow::Source& lau_window_source);
        virtual ~LauWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render(bool vsync) override;
        virtual std::weak_ptr<ILauWindow> create_window() override;
    private:
        std::vector<std::shared_ptr<ILauWindow>> _windows;
        std::vector<std::weak_ptr<ILauWindow>> _closing_windows;
        ILauWindow::Source _lau_window_source;
        TokenStore _token_store;
    };
}
