#include "Size.h"

namespace trview
{
    Size::Size()
        : Size(0,0)
    {
    }

    Size::Size(float width, float height)
        : width(width), height(height)
    {
    }
}
