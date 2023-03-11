#include "TokenStore.h"

namespace trview
{
    void TokenStore::clear()
    {
        _tokens.clear();
    }

    TokenStore& TokenStore::operator += (EventBase::Token&& token)
    {
        _tokens.emplace_back(std::move(token));
        return *this;
    }
}
