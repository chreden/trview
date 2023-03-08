#include <trview.app/Plugins/Plugin.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.tests.common/Mocks.hpp>
#include <trview.app/Mocks/Lua/ILua.h>
#include <trview.app/Mocks/IApplication.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    std::vector<uint8_t> to_bytes(const std::string& value)
    {
        return std::vector<uint8_t>(value.begin(), value.end());
    }
}

TEST(Plugin, ManifestLoaded)
{
    auto files = mock_shared<MockFiles>();
    ON_CALL(*files, load_file("test\\manifest.json"))
        .WillByDefault(testing::Return(to_bytes("{\"name\":\"Test Plugin\",\"author\":\"Test Author\",\"description\":\"Test Description\"}")));

    Plugin plugin(files, mock_unique<MockLua>(), "test");

    ASSERT_EQ(plugin.name(), "Test Plugin");
    ASSERT_EQ(plugin.author(), "Test Author");
    ASSERT_EQ(plugin.description(), "Test Description");
}

TEST(Plugin, Initialise)
{
    auto application = mock_unique<MockApplication>();
    auto [lua_ptr, lua] = create_mock<MockLua>();
    EXPECT_CALL(lua, initialise(application.get()));

    Plugin plugin(mock_shared<MockFiles>(), std::move(lua_ptr), "test");
    plugin.initialise(application.get());
}

TEST(Plugin, Execute)
{
    auto [lua_ptr, lua] = create_mock<MockLua>();
    EXPECT_CALL(lua, execute("test"));

    Plugin plugin(mock_shared<MockFiles>(), std::move(lua_ptr), "test");
    plugin.execute("test");
}

TEST(Plugin, PrintRegistered)
{
    auto [lua_ptr, lua] = create_mock<MockLua>();

    Plugin plugin(mock_shared<MockFiles>(), std::move(lua_ptr), "test");

    lua.on_print("test");

    ASSERT_EQ(plugin.messages(), "test");
}

TEST(Plugin, AddAndClearMessages)
{
    Plugin plugin(mock_shared<MockFiles>(), mock_unique<MockLua>(), "test");

    plugin.add_message("test");
    plugin.add_message("test2");
    ASSERT_EQ(plugin.messages(), "test\ntest2");
    plugin.clear_messages();
    ASSERT_EQ(plugin.messages(), "");
}

TEST(Plugin, DoFile)
{
    auto [lua_ptr, lua] = create_mock<MockLua>();
    EXPECT_CALL(lua, do_file("test.lua")).Times(1);

    Plugin plugin(mock_shared<MockFiles>(), std::move(lua_ptr), "test");
    plugin.do_file("test.lua");
}


TEST(Plugin, Reload)
{
    auto [lua_ptr, lua] = create_mock<MockLua>();
    EXPECT_CALL(lua, do_file("test\\plugin.lua")).Times(1);

    Plugin plugin(mock_shared<MockFiles>(), std::move(lua_ptr), "test");
    plugin.reload();
}
