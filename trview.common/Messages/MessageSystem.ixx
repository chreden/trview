export module trview.common:MessageSystem;

import :IMessageSystem;
import :IRecipient;

import std;

namespace trview
{
    export class MessageSystem final : public IMessageSystem
    {
    public:
        virtual ~MessageSystem() = default;
        void send_message(const Message& message) override;
        void add_recipient(const std::weak_ptr<IRecipient>& recipient) override;
    private:
        std::vector<std::weak_ptr<IRecipient>> _recipients;
    };
}
