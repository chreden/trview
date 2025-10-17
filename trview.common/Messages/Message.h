#pragma once

#include <memory>
#include <string>

namespace trview
{
    struct IMessageData
    {
        virtual ~IMessageData() = 0;
    };

    template <typename T>
    struct MessageData : public IMessageData
    {
        virtual ~MessageData() = default;

        explicit MessageData(const T& v)
            : value(v)
        {
        }

        T value;
    };

    struct Message
    {
        std::string type;
        std::shared_ptr<IMessageData> data;
    };
}
