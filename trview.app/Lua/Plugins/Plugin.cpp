#include "Plugin.h"
#include "../ImGui/ImGui.h"
#include "../trview/trview.h"

namespace trview
{
    IPlugin::~IPlugin()
    {
    }

    Plugin::Plugin(const std::vector<uint8_t>& data)
        : _data(data)
    {
        _state = luaL_newstate();
        luaL_openlibs(_state);
        lua::imgui_register(_state);
        lua::trview_register(_state);
    }

    Plugin::~Plugin()
    {
        lua_close(_state);
        _state = nullptr;
    }

    void Plugin::initialise()
    {
        std::string script(_data.begin(), _data.end());
        luaL_dostring(_state, script.c_str());
    }

    void Plugin::render()
    {
    }

    void Plugin::render_ui()
    {
        lua_getglobal(_state, "render_ui");
        if (lua_pcall(_state, 0, 0, 0) != LUA_OK)
        {
            std::string msg = lua_tostring(_state, -1);
            msg += "a";
        }
    }
}