#include "../../Geometry/IMesh.h"

struct lua_State;

namespace trview
{
    struct Triangle;

    namespace lua
    {
        int create_triangle(lua_State* L, const Triangle& triangle);
        void triangle_register(lua_State* L);
    }
}
