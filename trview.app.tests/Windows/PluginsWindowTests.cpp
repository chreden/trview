#include <trview.app/Windows/Plugins/PluginsWindow.h>
#include <trview.app/Mocks/Plugins/IPlugin.h>
#include <trview.app/Mocks/Plugins/IPlugins.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <format>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using testing::Return;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IPlugins> plugins{ mock_shared<MockPlugins>() };
            std::shared_ptr<IShell> shell{ mock_shared<MockShell>() };

            std::unique_ptr<PluginsWindow> build()
            {
                return std::make_unique<PluginsWindow>(plugins, shell);
            }

            test_module& with_plugins(std::shared_ptr<IPlugins> plugins)
            {
                this->plugins = plugins;
                return *this;
            }

            test_module& with_shell(std::shared_ptr<IShell> shell)
            {
                this->shell = shell;
                return *this;
            }
        };

        return test_module{};
    }
}

TEST(PluginsWindow, DirectoryOpened)
{
    auto plugin = mock_shared<MockPlugin>();
    ON_CALL(*plugin, name).WillByDefault(Return("Plugin Name"));
    ON_CALL(*plugin, path).WillByDefault(Return("test_path"));

    auto plugins = mock_shared<MockPlugins>();
    ON_CALL(*plugins, plugins).WillByDefault(Return(std::vector<std::weak_ptr<IPlugin>> { plugin }));

    auto shell = mock_shared<MockShell>();
    EXPECT_CALL(*shell, open(std::wstring(L"test_path"))).Times(1);

    auto window = register_test_module()
        .with_plugins(plugins)
        .with_shell(shell)
        .build();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Plugins 0")
        .push(PluginsWindow::Names::)
        .id(std::format("Open##{}", "Plugin Name").c_str()));
}
