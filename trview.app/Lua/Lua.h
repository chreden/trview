#pragma once

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <string>
#include <trview.common/Event.h>
#include <functional>
#include "ILua.h"

namespace trview
{
    struct IApplication;

    class Lua final : public ILua
    {
    public:
        Lua();
        ~Lua();
        void execute(const std::string& command) override;
        void initialise(IApplication* application) override;
    private:
        lua_State* L{ nullptr };
    };

    namespace lua
    {
        int push_string(lua_State* L, const std::string& text);

        template <typename Func>
        int push_list_p(lua_State* L, std::ranges::input_range auto&& range, Func&& func);

        template <typename Func>
        int push_list(lua_State* L, std::ranges::input_range auto&& range, Func&& func);

        template <typename T>
        T* get_self(lua_State* L);
    }
}

#include "Lua.inl"