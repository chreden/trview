#pragma once

#include <trview.common/MessageHandler.h>
#include <thread>

namespace trview
{
    class UpdateChecker final : public MessageHandler
    {
    public:
        explicit UpdateChecker(const Window& window);
        virtual ~UpdateChecker() = default;
        void check_for_updates();
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;
    private:
        std::thread _thread;
    };
}

