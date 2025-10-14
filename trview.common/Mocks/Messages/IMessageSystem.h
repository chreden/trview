#pragma once

#include "../../Messages/IMessageSystem.h"

namespace trview
{
    namespace mocks
    {
        struct MockMessageSystem : public IMessageSystem
        {
            MockMessageSystem();
            virtual ~MockMessageSystem();
            MOCK_METHOD(void, add_recipient, (const std::weak_ptr<IRecipient>&), (override));
            MOCK_METHOD(void, send_message, (const Message&), (override));
        };
    }
}