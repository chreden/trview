export module trview.common:IRecipient;

import :Message;

namespace trview
{
    export struct IRecipient
    {
        virtual ~IRecipient() = 0;
        virtual void receive_message(const Message& message) = 0;
    };
}
