#pragma once

#include <external/lua/src/lua.hpp>
#include <string>
#include <trview.common/Event.h>

namespace trview
{
    class Lua final
    {
    public:
        Lua();

        void execute(const std::string& command);

        void print(const std::string& value);

        Event<std::wstring> on_text;
    private:
        lua_State* _lua;
    };
}
