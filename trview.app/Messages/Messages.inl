#pragma once

#include <trview.common/Messages/IMessageSystem.h>

namespace trview
{
    namespace messages
    {
        template <typename T>
        void reply_to(const Message& message, const std::string& type, T&& data)
        {
            if (auto requester = std::static_pointer_cast<MessageData<std::weak_ptr<IRecipient>>>(message.data)->value.lock())
            {
                requester->receive_message({ .type = type, .data = std::make_shared<MessageData<std::remove_reference_t<T>>>(data) });
            }
        }
    }
}
