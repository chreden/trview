#pragma once

#include "ILayout.h"

namespace trview
{
    namespace ui
    {
        class GridLayout final : public ILayout
        {
        public:
            virtual ~GridLayout() = default;
            virtual void bind(Control& control) override;
        };
    }
}
