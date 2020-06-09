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

        void set_visible(bool value);
    private:
        TokenStore _token_store;
        ui::Window* _window;
        ui::TextArea* _input;
    };
}
