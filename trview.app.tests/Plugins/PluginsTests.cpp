#include <trview.app/Plugins/Plugins.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.tests.common/Mocks.hpp>
#include <trview.app/Mocks/Plugins/IPlugin.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

TEST(Plugins, PluginsLoaded)
{
    auto files = mock_shared<MockFiles>();

    std::optional<std::string> raised;
    auto source = [&](auto&& path) 
    {
        raised = path;
        return mock_unique<MockPlugin>(); 
    };
    UserSettings settings{};
    settings.plugin_directories.push_back("dir");

    EXPECT_CALL(*files, get_directories("dir"))
        .WillRepeatedly(testing::Return(std::vector<IFiles::Directory>{ { "plugindir", "plugindir_friendly" } }));

    Plugins plugins(files, source, settings);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "plugindir");
}
