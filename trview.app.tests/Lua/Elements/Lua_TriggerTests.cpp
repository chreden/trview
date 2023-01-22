#include <trview.app/Lua/Elements/Trigger/Lua_Trigger.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.hpp>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Trigger, Number)
{
    auto trigger = mock_shared<MockTrigger>()->with_number(123);

    lua_State* L = luaL_newstate();
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}
