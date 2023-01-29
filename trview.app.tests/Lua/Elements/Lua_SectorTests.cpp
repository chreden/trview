#include <trview.app/Lua/Elements/Sector/Lua_Sector.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Sector, Number)
{
    auto sector = mock_shared<MockSector>()->with_id(123);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}
