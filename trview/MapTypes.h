#pragma once

#include <SimpleMath.h>
#include <vector> 
#include <memory>
#include <algorithm>

using namespace DirectX::SimpleMath;

namespace trview
{
    enum class FunctionType
    {
        NORMAL, PORTAL, FLOOR_SLANT, CEILING_SLANT,
        TRIGGER, KILL, CLIMBABLE
    };

    struct Function
    {
        Function(const FunctionType& p_type) { type = p_type; }
        FunctionType type; 
    };

    struct FloorData
    {
        int row, column; 
        std::uint16_t floor, ceiling; 

        std::vector<Function> functions; 

        inline bool has_function(const FunctionType& ft) const
        {
            auto ptr = std::find_if(functions.begin(), functions.end(), [&ft] (const Function& f) throw() {
                return f.type == ft;
            });

            return ptr != std::end(functions);
        }
    };

    struct FunctionColour
    {
        FunctionType function;
        Color colour;
    };

    const static std::vector<FunctionColour> default_colours = {
        FunctionColour { FunctionType::NORMAL, Color(0.0, 0.7, 0.7) },
        FunctionColour { FunctionType::PORTAL, Color(0.0, 0.0, 0.0) },
        FunctionColour { FunctionType::TRIGGER, Color(1.0, 0.3, 0.7) },
        FunctionColour { FunctionType::KILL, Color(0.8, 0.2, 0.2) },
    }; 
}