#include "MessageSystem.h"
#include "IRecipient.h"

namespace trview
{
    IMessageData::~IMessageData()
    {
    }

    IMessageSystem::~IMessageSystem()
    {
    }

    IRecipient::~IRecipient()
    {
    }

    void MessageSystem::send_message(const Message& message)
    {
        // TODO: Prune the dead
        // TODO: Async?

        for (const auto& recipient : _recipients)
        {
            if (auto recipient_ptr = recipient.lock())
            {
                recipient_ptr->receive_message(message);
            }
        }
    }

    void MessageSystem::add_recipient(const std::weak_ptr<IRecipient>& recipient)
    {
        _recipients.push_back(recipient);
    }
}
