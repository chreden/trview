#pragma once

#include "ILayout.h"

namespace trview
{
    namespace ui
    {
        class FreeLayout final : public ILayout
        {
        public:
            virtual ~FreeLayout() = default;
            virtual void bind(Control& control) override;
        };
    }
}
