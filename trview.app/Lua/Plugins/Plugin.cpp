#include "Plugin.h"
#include "../ImGui/ImGui.h"
#include "../trview/trview.h"

namespace trview
{
    IPlugin::~IPlugin()
    {
    }

    Plugin::Plugin(const std::shared_ptr<ILog>& log, const std::string& name, const std::vector<uint8_t>& data)
        : _log(log), _name(name), _data(data)
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
        if (luaL_dostring(_state, script.c_str()) != LUA_OK)
        {
            error("initialise");
        }
    }

    void Plugin::render()
    {
        if (_status != Status::OK)
        {
            return;
        }
    }

    void Plugin::render_ui()
    {
        if (_status != Status::OK)
        {
            return;
        }

        lua_getglobal(_state, "render_ui");
        if (lua_pcall(_state, 0, 0, 0) != LUA_OK)
        {
            error("render_ui");
        }
    }

    void Plugin::error(const std::string& activity)
    {
        std::string msg = lua_tostring(_state, -1);
        _log->log(Message::Status::Error, "Plugins", { _name, activity }, msg);
        _log->log(Message::Status::Information, "Plugins", { _name, activity }, "Plugin is in error status, will no longer run");
        _status = Status::Error;
    }
}