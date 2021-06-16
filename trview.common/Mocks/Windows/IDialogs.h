#pragma once

#include "../../Windows/IDialogs.h"

namespace trview
{
    namespace mocks
    {
        struct MockDialogs final : public IDialogs
        {
            virtual ~MockDialogs() = default;
            MOCK_METHOD(bool, message_box, (const Window&, const std::wstring&, const std::wstring&, Buttons), (override));
        };
    }
}
