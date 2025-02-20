#include <trview.app/Plugins/Plugins.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.tests.common/Mocks.hpp>
#include <trview.app/Mocks/Plugins/IPlugin.h>
#include <trview.app/Mocks/IApplication.h>

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

    Plugins plugins(files, mock_shared<MockPlugin>(), source, settings);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "plugindir");
}

TEST(Plugins, Initialise)
{
    auto files = mock_shared<MockFiles>();
    auto [plugin_ptr, plugin] = create_mock<MockPlugin>();
    auto source = [&](auto&&...) { return std::move(plugin_ptr); };
    UserSettings settings{};
    settings.plugin_directories.push_back("dir");
    EXPECT_CALL(*files, get_directories("dir"))
        .WillRepeatedly(testing::Return(std::vector<IFiles::Directory>{ { "plugindir", "plugindir_friendly" } }));

    Plugins plugins(files, mock_shared<MockPlugin>(), source, settings);

    auto application = mock_unique<MockApplication>();
    EXPECT_CALL(plugin, initialise(application.get())).Times(1);
    plugins.initialise(application.get());
}

TEST(Plugins, SettingsApplied)
{
    auto files = mock_shared<MockFiles>();
    auto plugin = mock_shared<MockPlugin>();
    EXPECT_CALL(*plugin, set_enabled(false)).Times(1);

    auto source = [&](auto&&...) { return plugin; };
    UserSettings settings{ .plugins = { { "plugindir", {.enabled = false } } } };
    settings.plugin_directories.push_back("dir");
    EXPECT_CALL(*files, get_directories("dir"))
        .WillRepeatedly(testing::Return(std::vector<IFiles::Directory>{ { "plugindir", "plugindir_friendly" } }));

    Plugins plugins(files, mock_shared<MockPlugin>(), source, settings);
}
