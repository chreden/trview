module;
module trview.common:MessageSystem;

namespace trview
{
    void MessageSystem::send_message(const Message& message)
    {
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
