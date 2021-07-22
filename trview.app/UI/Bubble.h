#pragma once

#include "IBubble.h"
#include <trview.ui/Label.h>

namespace trview
{
    class Bubble final : public IBubble
    {
    public:
        static const float FadeTime;

        struct Names
        {
            static const std::string Bubble;
        };

        explicit Bubble(ui::Control& control);
        virtual ~Bubble() = default;
        virtual void show(const Point& position) override;
    private:
        TokenStore _token_store;
        ui::Label* _label{ nullptr };
    };
}