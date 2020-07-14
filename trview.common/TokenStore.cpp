#include "stdafx.h"
#include "TokenStore.h"

namespace trview
{
    TokenStore& TokenStore::operator += (EventBase::Token&& token)
    {
        _tokens.emplace_back(std::move(token));
        return *this;
    }
}
