#pragma once

namespace trview
{
    enum class LevelSortingMode
    {
        Full,
        NameThenFilename,
        FilenameOnly
    };

    constexpr std::string to_string(LevelSortingMode mode) noexcept;
}

#include "LevelSortingMode.hpp"
