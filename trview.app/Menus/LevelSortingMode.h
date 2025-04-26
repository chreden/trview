#pragma once

namespace trview
{
    enum class LevelSortingMode
    {
        NameThenFilename,
        FilenameOnly
    };

    constexpr std::string to_string(LevelSortingMode mode) noexcept;
}

#include "LevelSortingMode.hpp"
