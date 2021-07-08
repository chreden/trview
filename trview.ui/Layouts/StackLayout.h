#pragma once

#include "ILayout.h"

namespace trview
{
    namespace ui
    {
        class StackLayout final : public ILayout
        {
        public:
            virtual ~StackLayout() = default;
        };
    }
}