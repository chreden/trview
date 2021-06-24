#pragma once

#include "../../Windows/IDialogs.h"

namespace trview
{
    namespace mocks
    {
        struct MockDialogs final : public IDialogs
        {
            virtual ~MockDialogs() = default;
            MOCK_METHOD(bool, message_box, (const Window&, const std::wstring&, const std::wstring&, Buttons), (const, override));
            MOCK_METHOD(std::optional<std::string>, open_file, (const std::wstring&, const std::wstring&, const std::vector<std::wstring>&, uint32_t), (const, override));
        };
    }
}
