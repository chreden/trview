export module trview.common:Message;

import std;

namespace trview
{
    export struct IMessageData
    {
        virtual ~IMessageData() = 0;
    };

    export template <typename T>
    struct MessageData : public IMessageData
    {
        virtual ~MessageData() = default;

        explicit MessageData(const T& v)
            : value(v)
        {
        }

        T value;
    };

    export struct Message
    {
        std::string type;
        std::shared_ptr<IMessageData> data;
    };
}
