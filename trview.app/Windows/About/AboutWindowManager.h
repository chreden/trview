#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>
#include "IAboutWindowManager.h"
#include "IAboutWindow.h"

namespace trview
{
    struct IAboutWindow;

    class AboutWindowManager final : public IAboutWindowManager, public MessageHandler
    {
    public:
        explicit AboutWindowManager(const Window& window, const IAboutWindow::Source& about_window_source);
        virtual ~AboutWindowManager() = default;
        void create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
    private:
        TokenStore _token_store;
        IAboutWindow::Source _about_window_source;
        std::shared_ptr<IAboutWindow> _about_window;
        bool _closing{ false };
    };
}
