#pragma once

#include <SimpleMath.h>
#include <vector> 
#include <memory>
#include <algorithm>

#include <trview.ui\Point.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    enum class FunctionType
    {
        NONE=-1, NORMAL, PORTAL, FLOOR_SLANT, CEILING_SLANT,
        TRIGGER, KILL, CLIMBABLE, WALL=0x20
    };

    struct Floor 
    {
        Floor(const FunctionType& func) : function(func) {}

        FunctionType function; 
        std::uint16_t portal_to = 0; 
    };
}