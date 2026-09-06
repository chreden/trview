module;

#include <SimpleMath.h>
#include <external/lua/src/lua.h>

export module trview.app:LuaMatrix;

namespace trview
{
    namespace lua
    {
        export int create_matrix(lua_State* L, const DirectX::SimpleMath::Matrix& value);
        export bool is_matrix(lua_State* L, int index);
        export void matrix_register(lua_State* L);
    }
}
