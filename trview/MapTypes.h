#pragma once

#include <SimpleMath.h>
#include <vector> 
#include <memory>
#include <algorithm>

#include <trview.ui\Point.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    enum class Function
    {
        NORMAL, PORTAL, FLOOR_SLANT, CEILING_SLANT,
        TRIGGER, KILL, CLIMBABLE, WALL=0x20
    };

    struct Floor 
    {
        Floor(const Function& func) : function(func) {}
        Function function; 
    };

    struct Sector
    {
        std::vector<Floor> floor_data;
        std::int8_t floor, ceiling;
        int row, column; 
    };

    struct MapTile
    {
        // Where to draw the tile on screen 
        // origin = top left corner, last = bottom right corner 
        ui::Point origin, last; 

        // Color of the tile 
        Color color;

        // Sector this tile points to 
        Sector sector; 
    };
}