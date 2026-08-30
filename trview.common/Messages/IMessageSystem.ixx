export module trview.common:IMessageSystem;

import :Message;
import :IRecipient;

namespace trview
{
    export struct IMessageSystem
    {
        virtual ~IMessageSystem() = 0;
        virtual void add_recipient(const std::weak_ptr<IRecipient>& recipient) = 0;
        virtual void send_message(const Message& message) = 0;
    };
}
