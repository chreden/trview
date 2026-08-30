module;

#include <gmock/gmock.h>

export module trview.common:MockClipboard;

import :IClipboard;
import std;

namespace trview
{
    namespace mocks
    {
        export struct MockClipboard : public IClipboard
        {
            MockClipboard();
            virtual ~MockClipboard();
            MOCK_METHOD(std::wstring, read, (), (const));
            MOCK_METHOD(void, write, (const std::wstring&));
        };
    }
}
