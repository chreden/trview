#pragma once

#include <trview.common/MessageHandler.h>
#include "IWindowTester.h"

namespace trview
{
    namespace input
    {
        class WindowTester final : public MessageHandler, public IWindowTester
        {
        public:
            explicit WindowTester(const Window& window);
            virtual ~WindowTester() = default;
            virtual bool is_window_under_cursor() const override;
            virtual int screen_width(bool rdp) const override;
            virtual int screen_height(bool rdp) const override;
            virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        private:
            bool _mouse_in_window{ false };
        };
    }
}
