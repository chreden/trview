#pragma once

#include <trview.ui/Window.h>
#include <trview.ui/TextArea.h>

namespace trview
{
    class Console final
    {
    public:
        explicit Console(ui::Control& parent);

        bool visible() const;

        void print(const std::wstring& text);

        void set_visible(bool value);

        Event<std::wstring> on_command;
    private:
        TokenStore _token_store;
        ui::Window* _window;
        ui::TextArea* _log;
        ui::TextArea* _input;
    };
}
