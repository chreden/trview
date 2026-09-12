module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaCamera;

import trlevel;
import trview.common;
import trview.lua;

import :Lua;
import :LuaVector3;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int camera_metatable = LUA_NOREF;

            const std::unordered_map<std::string, ICamera::Mode> modes
            {
                { "free", ICamera::Mode::Free },
                { "axis", ICamera::Mode::Axis },
                { "orbit", ICamera::Mode::Orbit }
            };

            const std::unordered_map<ICamera::Mode, std::string> modes_to_string
            {
                { ICamera::Mode::Free, "free" },
                { ICamera::Mode::Axis, "axis" },
                { ICamera::Mode::Orbit, "orbit" }
            };

            std::optional<ICamera::Mode> to_mode(lua_State* L, int index)
            {
                if (LUA_TSTRING != lua_type(L, index))
                {
                    return std::nullopt;
                }

                const std::string value = lua_tostring(L, index);
                const auto result = modes.find(value);
                if (result == modes.end())
                {
                    return std::nullopt;
                }
                return result->second;
            }

            std::string to_string(ICamera::Mode mode)
            {
                const auto result = modes_to_string.find(mode);
                return result == modes_to_string.end() ? "unknown" : result->second;
            }

            int camera_index(lua_State* L)
            {
                auto camera = get_userdata<std::shared_ptr<ICamera>>(L, 1);
                const std::string key = lua_tostring(L, 2);

                if (key == "mode")
                {
                    lua_pushstring(L, to_string(camera->mode()).c_str());
                    return 1;
                }
                else if (key == "target")
                {
                    return create_vector3(L, camera->target() * trlevel::Scale);
                }

                return 0;
            }

            int camera_newindex(lua_State* L)
            {
                auto camera = get_userdata<std::shared_ptr<ICamera>>(L, 1);
                const std::string key = lua_tostring(L, 2);

                if (key == "mode")
                {
                    const auto new_mode = to_mode(L, 3);
                    if (new_mode)
                    {
                        camera->set_mode(new_mode.value());
                    }
                }
                else if (key == "target")
                {
                    if (lua::is_vector3(L, 3))
                    {
                        camera->set_target(to_vector3(L, 3) / trlevel::Scale);
                        return 0;
                    }

                    const int type = lua_type(L, 3);
                    if (equals_any(type, LUA_TUSERDATA, LUA_TTABLE))
                    {
                        lua_getfield(L, -1, "position");
                        if (is_vector3(L, -1))
                        {
                            camera->set_target(to_vector3(L, -1) / trlevel::Scale);
                            return 0;
                        }
                        else
                        {
                            lua_pop(L, 1);
                        }
                    }
                }

                return 0;
            }
        }

        void camera_register(lua_State* L)
        {
            camera_metatable = store_metatable(L,
                {
                    { "__index", camera_index },
                    { "__newindex", camera_newindex },
                    { "__gc", default_gc<std::shared_ptr<ICamera>> }
                });
        }

        int create_camera(lua_State* L, const std::shared_ptr<ICamera>& camera)
        {
            create_userdata(L, camera);
            assign_metatable(L, camera_metatable);
            return 1;
        }

        std::shared_ptr<ICamera> to_camera(lua_State* L, int index)
        {
            return get_userdata<std::shared_ptr<ICamera>>(L, index);
        }
    }
}
