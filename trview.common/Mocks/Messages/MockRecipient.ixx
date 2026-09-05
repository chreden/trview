module;

#include <gmock/gmock.h>

export module trview.common:MockRecipient;

import :IRecipient;

namespace trview
{
    namespace mocks
    {
        export struct MockRecipient : public IRecipient
        {
            MOCK_METHOD(void, receive_message, (const Message&), (override));
        };
    }
}
