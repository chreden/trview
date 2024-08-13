#include "Lua_Camera.h"
#include "../Lua.h"
#include "../Vector3.h"

#include "../../Elements/IItem.h"
#include "../Elements/Item/Lua_Item.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
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
                auto camera = get_self<ICamera>(L);
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
                auto camera = get_self<ICamera>(L);
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
                    camera->set_mode(ICamera::Mode::Orbit);

                    const int type = lua_type(L, 3);
                    if (equals_any(type, LUA_TUSERDATA, LUA_TTABLE))
                    {
                        if (LUA_TTABLE == lua_getfield(L, -1, "position"))
                        {
                            camera->set_target(to_vector3(L, -1) / trlevel::Scale);
                            return 0;
                        }
                        else
                        {
                            lua_pop(L, 1);
                        }
                    }
                    else if (type == LUA_TTABLE)
                    {
                        // Assume top is a vector3 if metatable lookups failed.
                        camera->set_target(to_vector3(L, 3) / trlevel::Scale);
                    }
                }

                return 0;
            }
        }

        void camera_register(lua_State*)
        {
        }

        int create_camera(lua_State* L, const std::shared_ptr<ICamera>& camera)
        {
            return create(L, camera, camera_index, camera_newindex);
        }

        std::shared_ptr<ICamera> to_camera(lua_State* L, int index)
        {
            return get_self<ICamera>(L, index);
        }
    }
}
