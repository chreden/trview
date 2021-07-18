#pragma once

#include "IBubble.h"
#include <trview.ui/Control.h>
#include <trview.ui/Label.h>

namespace trview
{
    class Bubble final : public IBubble
    {
    public:
        explicit Bubble(ui::Control& control);
        virtual ~Bubble() = default;
        void show(const Point& position);
    private:
        TokenStore _token_store;
        ui::Label* _label{ nullptr };
    };
}