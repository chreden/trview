#pragma once

#include <trview.ui/Window.h>

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
    };
}
