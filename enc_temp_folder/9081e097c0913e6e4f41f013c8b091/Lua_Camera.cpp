#include "Lua_Camera.h"
#include "../Lua.h"
#include "../Vector3.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int camera_index(lua_State* L)
            {
                auto camera = get_self<ICamera>(L);
                const std::string key = lua_tostring(L, 2);

                return 0;
            }

            int camera_newindex(lua_State* L)
            {
                auto camera = get_self<ICamera>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "target")
                {
                    camera->set_mode(ICamera::Mode::Orbit);
                    camera->set_target(to_vector3(L, -1) / trlevel::Scale);
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
