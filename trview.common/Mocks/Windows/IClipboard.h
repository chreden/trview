#pragma once

#include "../../Windows/IClipboard.h"

namespace trview
{
    struct MockClipboard final : public IClipboard
    {
        virtual ~MockClipboard() = default;
        MOCK_METHOD(std::wstring, read, (const Window&), (const));
        MOCK_METHOD(void, write, (const Window&, const std::wstring&));
    };
}
