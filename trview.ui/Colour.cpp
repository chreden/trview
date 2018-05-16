#include "Colour.h"

namespace trview
{
    namespace ui
    {
        Colour::Colour(float a, float r, float g, float b)
            : a(a), r(r), g(g), b(b)
        {
        }

        Colour::operator DirectX::SimpleMath::Color() const
        {
            return DirectX::SimpleMath::Color(r, g, b, a);
        }
    }
}