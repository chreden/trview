#pragma once

#include "../../Windows/IDialogs.h"

namespace trview
{
    namespace mocks
    {
        struct MockDialogs : public IDialogs
        {
            virtual ~MockDialogs() = default;
            MOCK_METHOD(bool, message_box, (const std::wstring&, const std::wstring&, Buttons), (const, override));
            MOCK_METHOD(std::optional<FileResult>, open_file, (const std::wstring&, const std::vector<FileFilter>&, uint32_t), (const, override));
            MOCK_METHOD(std::optional<FileResult>, save_file, (const std::wstring&, const std::vector<FileFilter>&, uint32_t), (const, override));
        };
    }
}
