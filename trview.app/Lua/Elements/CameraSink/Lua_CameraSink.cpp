module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaCameraSink;

import trview.lua;

import :Lua;
import :LuaRoom;
import :LuaVector3;
import :LuaTrigger;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int camera_sink_metatable = LUA_NOREF;

            int camera_sink_newindex(lua_State* L)
            {
                auto camera_sink = lua::get_userdata<std::shared_ptr<ICameraSink>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "type")
                {
                    if (auto level = camera_sink->level().lock())
                    {
                        const char* type_str = lua_tostring(L, -1);
                        if (type_str)
                        {
                            std::string type = type_str;
                            if (type == "Camera")
                            {
                                camera_sink->set_type(ICameraSink::Type::Camera);
                            }
                            else if (type == "Sink")
                            {
                                camera_sink->set_type(ICameraSink::Type::Sink);
                            }
                            else
                            {
                                return luaL_error(L, "%s is not a valid type for Camera/Sink. Valid values are \"Camera\" and \"Sink\"", type.c_str());
                            }
                        }
                        else
                        {
                            return luaL_error(L, "nil is not a valid type for Camera/Sink. Valid values are \"Camera\" and \"Sink\"");
                        }
                    }
                }
                else if (key == "visible")
                {
                    camera_sink->set_visible(lua_toboolean(L, -1));
                }

                return 0;
            }

            DirectX::SimpleMath::Vector3 scale_vector(const DirectX::SimpleMath::Vector3& v)
            {
                return v * trlevel::Scale;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "box_index", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::box_index> },
                { "flag", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::flag> },
                { "inferred_rooms", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::inferred_rooms> },
                { "number", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::number> },
                { "persistent", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::persistent> },
                { "position", prop_getter_with_transform<std::shared_ptr<ICameraSink>, &ICameraSink::position, scale_vector> },
                { "room", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::room> },
                { "strength", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::strength> },
                { "type", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::type> },
                { "triggered_by", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::triggers> },
                { "trigger_references", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::triggers> },
                { "visible", prop_getter<std::shared_ptr<ICameraSink>, &ICameraSink::visible> },
            };
        }

        void camera_sink_register(lua_State* L)
        {
            camera_sink_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions> },
                    { "__newindex", camera_sink_newindex },
                    { "__gc", default_gc<std::shared_ptr<ICameraSink>> }
                });
        }

        int to_lua(lua_State* L, const std::weak_ptr<ICameraSink>& camera_sink)
        {
            return create_userdata(L, camera_sink.lock(), camera_sink_metatable);
        }

        int to_lua(lua_State* L, ICameraSink::Type type)
        {
            lua_pushstring(L, to_string(type).c_str());
            return 1;
        }
    }
}

