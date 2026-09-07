export module trview.app:LevelSortingMode;

import std;

namespace trview
{
    export enum class LevelSortingMode
    {
        Full,
        NameThenFilename,
        FilenameOnly
    };

    export constexpr std::string to_string(LevelSortingMode mode) noexcept;
}

#include "LevelSortingMode.hpp"
