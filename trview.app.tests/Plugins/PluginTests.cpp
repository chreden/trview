#include <trview.app/Plugins/Plugin.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.tests.common/Mocks.hpp>
#include <trview.app/Mocks/Lua/ILua.h>

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
