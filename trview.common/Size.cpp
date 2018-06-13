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

    Size& Size::operator+=(const Size& other) 
    {
        width += other.width;
        height += other.height;
        return *this;
    }

    Size Size::operator+(const Size& other) const
    {
        return Size(width + other.width, height + other.height);
    }
}
