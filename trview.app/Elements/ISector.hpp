#pragma once

namespace trview
{
    constexpr std::string to_string(ISector::TriangulationDirection direction)
    {
        switch (direction)
        {
        case ISector::TriangulationDirection::NeSw:
            return "NESW";
        case ISector::TriangulationDirection::NwSe:
            return "NWSE";
        }
        return "None";
    }
}