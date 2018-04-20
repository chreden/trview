#pragma once

#include <SimpleMath.h>
#include <vector> 

using namespace DirectX::SimpleMath;

namespace trview
{
    enum class Function
    {
        NORMAL, PORTAL, FLOOR_SLANT, CEILING_SLANT,
        TRIGGER, KILL, CLIMBABLE
    };

    struct FloorData
    {
    public:
        int row, column;
        Function function;
        std::uint16_t subfunction;

        std::int16_t floor, ceiling;
    };

    struct FunctionColour
    {
        Function function;
        Color colour;
    };

    const static std::vector<FunctionColour> default_colours = {
        FunctionColour { Function::NORMAL, Color(0.0, 0.7, 0.7) },
        FunctionColour { Function::PORTAL, Color(0.0, 0.0, 0.0) }, 
        FunctionColour { Function::TRIGGER, Color(1.0, 0.3, 0.7) },
        FunctionColour { Function::KILL, Color(0.8, 0.2, 0.2) },
    }; 
}