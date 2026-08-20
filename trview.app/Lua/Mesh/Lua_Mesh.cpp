#include "Lua_Mesh.h"
#include "../Lua.h"
#include <trview.lua/inc/tables.h>

namespace trview
{
    namespace lua
    {
        namespace
        {
            IMesh::Source mesh_source;
            int mesh_metatable = LUA_NOREF;

            int mesh_call(lua_State* L)
            {
                const std::vector<Triangle> triangles = get_list<Triangle>(L, 2, "triangles");
                auto mesh = mesh_source(triangles);
                return create_mesh(L, mesh);
            }

            int mesh_index(lua_State* L)
            {
                const auto self = get_userdata<std::shared_ptr<IMesh>>(L, 1);
                const std::string key = lua_tostring(L, 2);
                self;
                if (key == "test")
                {
                    lua_pushstring(L, "lmao");
                    return 1;
                }
                return 0;
            }
        }

        int create_mesh(lua_State* L, const std::shared_ptr<IMesh>& mesh)
        {
            create_userdata<std::shared_ptr<IMesh>>(L, mesh);
            assign_metatable(L, mesh_metatable);
            return 1;
        }

        void mesh_register(lua_State* L, const IMesh::Source& source)
        {
            mesh_source = source;

            mesh_metatable = store_metatable(L,
                {
                    { "__index", mesh_index },
                    { "__gc", default_gc<std::shared_ptr<IMesh>> }
                });

            lua_newtable(L);
            create_metatable(L,
                {
                    { "__call", mesh_call }
                });
            lua_setglobal(L, "Mesh");
        }
    }
}