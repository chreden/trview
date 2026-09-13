module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaStaticMesh;

import trlevel;
import trview.lua;
import :Lua;
import :LuaRoom;
import :LuaVector3;
import :LuaColour;
import :LuaBoundingBox;

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int static_mesh_metatable = LUA_NOREF;

            int static_mesh_newindex(lua_State* L)
            {
                auto static_mesh = lua::get_userdata<std::shared_ptr<IStaticMesh>>(L, 1);
                const std::string key = lua_tostring(L, 2);

                if (key == "visible")
                {
                    static_mesh->set_visible(lua_toboolean(L, -1));
                }
                return 0;
            }

            DirectX::SimpleMath::Vector3 scale_vector(const DirectX::SimpleMath::Vector3& v)
            {
                return v * trlevel::Scale;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "breakable", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::breakable> },
                { "collision", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::collision> },
                { "has_collision", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::has_collision> },
                { "id", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::id> },
                { "position", prop_getter_with_transform<std::shared_ptr<IStaticMesh>, &IStaticMesh::position, scale_vector> },
                { "room", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::room> },
                { "rotation", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::rotation> },
                { "type", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::type> },
                { "visible", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::visible> },
                { "visibility", prop_getter<std::shared_ptr<IStaticMesh>, &IStaticMesh::visibility> },
            };
        }

        void static_mesh_register(lua_State* L)
        {
            static_mesh_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions> },
                    { "__newindex", static_mesh_newindex },
                    { "__gc", default_gc<std::shared_ptr<IStaticMesh>> },
                });
        }

        int create_static_mesh(lua_State* L, const std::shared_ptr<IStaticMesh>& mesh)
        {
            return create_userdata(L, mesh, static_mesh_metatable);
        }

        int to_lua(lua_State* L, IStaticMesh::Type type)
        {
            lua_pushstring(L, to_string(type).c_str());
            return 1;
        }
    }
}
