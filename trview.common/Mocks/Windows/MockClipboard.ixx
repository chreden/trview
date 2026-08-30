export module trview.common:Mocks;

import :IClipboard;

namespace trview
{
    namespace mocks
    {
        struct MockClipboard : public IClipboard
        {
            MockClipboard();
            virtual ~MockClipboard();
            MOCK_METHOD(std::wstring, read, (), (const));
            MOCK_METHOD(void, write, (const std::wstring&));
        };
    }
}
