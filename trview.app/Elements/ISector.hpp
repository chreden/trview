#pragma once

namespace trview
{
    constexpr std::string to_string(TriangulationDirection direction)
    {
        switch (direction)
        {
        case TriangulationDirection::NeSw:
            return "NESW";
        case TriangulationDirection::NwSe:
            return "NWSE";
        }
        return "None";
    }
}