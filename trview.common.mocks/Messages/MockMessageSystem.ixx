module;

#include <gmock/gmock.h>

export module trview.common.mocks:MockMessageSystem;

import trview.common;

namespace trview
{
    namespace mocks
    {
        export struct MockMessageSystem : public IMessageSystem
        {
            MOCK_METHOD(void, add_recipient, (const std::weak_ptr<IRecipient>&), (override));
            MOCK_METHOD(void, send_message, (const Message&), (override));
        };
    }
}