export module trview.input:WindowTester;

import trview.common;
import :IWindowTester;

namespace trview
{
    namespace input
    {
        export class WindowTester final : public MessageHandler, public IWindowTester
        {
        public:
            explicit WindowTester(const Window& window);
            virtual ~WindowTester() = default;
            virtual bool is_window_under_cursor() const override;
            virtual int screen_width(bool rdp) const override;
            virtual int screen_height(bool rdp) const override;
            virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        private:
            bool _mouse_in_window{ false };
        };
    }
}
