module;

#include <gmock/gmock.h>

export module trview.common.mocks:MockRecipient;

import trview.common;

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
