#include "Lua_StaticMesh.h"
#include "../../Lua.h"
#include "../../Vector3.h"
#include "../../BoundingBox.h"
#include "../Room/Lua_Room.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int static_mesh_index(lua_State* L)
            {
                auto static_mesh = lua::get_self<IStaticMesh>(L);
                const std::string key = lua_tostring(L, 2);
                if (key == "breakable")
                {
                    lua_pushboolean(L, static_mesh->breakable());
                    return 1;
                }
                else if (key == "collision")
                {
                    return create_bounding_box(L, static_mesh->collision());
                }
                else if (key == "id")
                {
                    lua_pushinteger(L, static_mesh->id());
                    return 1;
                }
                else if (key == "position")
                {
                    return create_vector3(L, static_mesh->position() * trlevel::Scale);
                }
                else if (key == "room")
                {
                    return create_room(L, static_mesh->room().lock());
                }
                else if (key == "rotation")
                {
                    lua_pushnumber(L, static_mesh->rotation());
                    return 1;
                }
                else if (key == "type")
                {
                    lua_pushstring(L, to_string(static_mesh->type()).c_str());
                    return 1;
                }
                else if (key == "visible")
                {
                    lua_pushboolean(L, static_mesh->visible());
                    return 1;
                }
                else if (key == "visibility")
                {
                    return create_bounding_box(L, static_mesh->visibility());
                }
                return 0;
            }

            int static_mesh_newindex(lua_State* L)
            {
                auto static_mesh = lua::get_self<IStaticMesh>(L);
                const std::string key = lua_tostring(L, 2);

                if (key == "visible")
                {
                    static_mesh->set_visible(lua_toboolean(L, -1));
                }
                return 0;
            }
        }

        int create_static_mesh(lua_State* L, const std::shared_ptr<IStaticMesh>& mesh)
        {
            return create(L, mesh, static_mesh_index, static_mesh_newindex);
        }
    }
}
