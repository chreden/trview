module;

#include <SimpleMath.h>
#include <external/lua/src/lua.h>

export module trview.app:LuaVector2;

namespace trview
{
    namespace lua
    {
        export int to_lua(lua_State* L, const DirectX::SimpleMath::Vector2& value);
        export void vector2_register(lua_State* L);
    }
}

namespace DirectX
{
    namespace SimpleMath
    {
        export using trview::lua::to_lua;
    }
}

