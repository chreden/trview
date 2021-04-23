#pragma once

#include "../../IInput.h"

namespace trview
{
    namespace ui
    {
        namespace mocks
        {
            struct MockInput final : public IInput
            {
                virtual ~MockInput() = default;
                MOCK_METHOD(Control*, focus_control, (), (const));
                MOCK_METHOD(input::IMouse&, mouse, ());
            };
        }
    }
}

