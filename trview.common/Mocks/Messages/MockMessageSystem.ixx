module;

#include <gmock/gmock.h>

export module trview.common:MockMessageSystem;

import :IMessageSystem;

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