#pragma once

namespace trview
{
    constexpr std::string to_string(LevelSortingMode mode) noexcept
    {
        switch (mode)
        {
        case LevelSortingMode::NameThenFilename:
            return "By name, then filename";
        case LevelSortingMode::FilenameOnly:
            return "By filename only";
        }
        return "Unknown sorting mode";
    }
}
