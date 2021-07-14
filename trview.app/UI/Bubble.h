#pragma once

#include <trview.ui/Control.h>
#include <trview.ui/Label.h>

namespace trview
{
    class Bubble final
    {
    public:
        explicit Bubble(ui::Control& control);
        void show(const Point& position);
    private:
        TokenStore _token_store;
        ui::Label* _label{ nullptr };
    };
}