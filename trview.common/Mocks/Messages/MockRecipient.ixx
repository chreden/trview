module;

#include <gmock/gmock.h>

export module trview.common:MockRecipient;

import :IRecipient;

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
