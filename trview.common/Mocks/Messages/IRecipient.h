#pragma once

#include "../../Messages/IRecipient.h"

namespace trview
{
    namespace mocks
    {
        struct MockRecipient : public IRecipient
        {
            MockRecipient();
            virtual ~MockRecipient();
            MOCK_METHOD(void, receive_message, (const Message&), (override));
        };
    }
}
