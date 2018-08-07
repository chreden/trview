#include "TokenStore.h"

namespace trview
{
    void TokenStore::add(EventBase::Token&& token)
    {
        _tokens.emplace_back(std::move(token));
    }
}
