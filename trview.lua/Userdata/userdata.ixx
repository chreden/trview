module;

#include <external/lua/src/lua.h>

export module trview.lua:Userdata;

namespace trview
{
    export namespace lua
    {
        template <typename T>
        void create_userdata(lua_State* L, const T& value)
        {
            T* ptr = reinterpret_cast<T*>(lua_newuserdata(L, sizeof(T)));
            new(ptr) T(value);
        }

        template <typename T>
        T& get_userdata(lua_State* L, int index)
        {
            return *static_cast<T*>(lua_touserdata(L, index));
        }

        template <typename T>
        int default_gc(lua_State* L)
        {
            T* ptr = static_cast<T*>(lua_touserdata(L, 1));
            ptr->~T();
            return 0;
        }
    }
}

