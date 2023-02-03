#pragma once

#include <exception>

namespace trview
{
    struct UserCancelledException final : public std::exception
    {
    };
}
