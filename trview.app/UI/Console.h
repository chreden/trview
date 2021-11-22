#pragma once

#include <trview.ui/Window.h>
#include <trview.ui/TextArea.h>

namespace trview
{
    class Console final
    {
    public:
        struct Names
        {
            static const std::string log;
            static const std::string input;
        };

        explicit Console(ui::Control& parent, const ui::UiSource& ui_source);

        bool visible() const;

        void print(const std::wstring& text);

        void set_visible(bool value);

        Event<std::wstring> on_command;
    private:
        TokenStore _token_store;
        ui::Control* _window;
        ui::TextArea* _log;
        ui::TextArea* _input;
    };
}
