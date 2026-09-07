#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "../Lua.h"

import trview.app;
import trview.app.mocks;
import trview.tests.common;

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;

TEST(Lua_Mesh, Constructor)
{
    auto mesh = std::make_shared<MockMesh>();
    auto mesh_source = [&](auto&&...) { return mesh; };

    LuaState L;
    lua::triangle_register(L);
    lua::mesh_register(L, mesh_source);

    ASSERT_EQ(0, luaL_dostring(L, "x = Mesh({triangles = {}}) return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<std::shared_ptr<IMesh>>(L, -1), mesh);
}
