export module trview.common:TokenStore;

import std;
import :Event;

namespace trview
{
    class TokenStore
    {
    public:
        TokenStore& operator += (EventBase::Token&& token);
        void clear();
    private:
        std::vector<EventBase::Token> _tokens;
    };
}
