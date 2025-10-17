#pragma once

namespace trview
{
    struct Message;

    struct IRecipient
    {
        virtual ~IRecipient() = 0;
        virtual void receive_message(const Message& message) = 0;
    };
}
