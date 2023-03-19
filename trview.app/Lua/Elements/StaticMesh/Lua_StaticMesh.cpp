#include "Lua_StaticMesh.h"
#include "../../Lua.h"
#include "../../Vector3.h"
#include "../Room/Lua_Room.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<IStaticMesh**, std::shared_ptr<IStaticMesh>> static_meshes;

            int static_mesh_index(lua_State* L)
            {
                auto static_mesh = lua::get_self<IStaticMesh>(L);
                const std::string key = lua_tostring(L, 2);
                if (key == "position")
                {
                    return create_vector3(L, static_mesh->position() * trlevel::Scale);
                }
                else if (key == "room")
                {
                    return create_room(L, static_mesh->room().lock());
                }
                return 0;
            }

            int static_mesh_newindex(lua_State* L)
            {
                auto static_mesh = lua::get_self<IStaticMesh>(L);
                const std::string key = lua_tostring(L, 2);
                key;
                static_mesh;
                return 0;
            }

            int static_mesh_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
                IStaticMesh** userdata = static_cast<IStaticMesh**>(lua_touserdata(L, 1));
                static_meshes.erase(userdata);
                return 0;
            }
        }

        void create_static_mesh(lua_State* L, const std::shared_ptr<IStaticMesh>& mesh)
        {
            if (!mesh)
            {
                lua_pushnil(L);
                return;
            }

            IStaticMesh** userdata = static_cast<IStaticMesh**>(lua_newuserdata(L, sizeof(mesh.get())));
            *userdata = mesh.get();
            static_meshes[userdata] = mesh;

            lua_newtable(L);
            lua_pushcfunction(L, static_mesh_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, static_mesh_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, static_mesh_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
        }
    }
}
