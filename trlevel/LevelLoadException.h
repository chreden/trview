#pragma once

#include <exception>

namespace trlevel
{
    struct LevelLoadException final : public std::exception
    {
    };
}