#pragma once

#include <trview.ui/Button.h>

namespace trview
{
    class ActionMenu final
    {
    public:
        explicit ActionMenu(ui::Control& parent);

        void show(const Point& position);
    private:
        ui::Button* _test_button;
    };
}
