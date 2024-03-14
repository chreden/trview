#include "pch.h"
#include "PluginsWindowTests.h"
#include <trview.app/Windows/Plugins/PluginsWindow.h>
#include <trview.app/Mocks/Plugins/IPlugin.h>
#include <trview.app/Mocks/Plugins/IPlugins.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

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

    struct PluginsWindowContext final
    {
        std::shared_ptr<PluginsWindow> ptr;

        void render()
        {
            if (ptr)
            {
                ptr->render();
            }
        }
    };
}

void register_plugins_window_tests(ImGuiTestEngine* engine)
{
    test<PluginsWindowContext>(engine, "Plugins Window", "Directory Opened",
        [](ImGuiTestContext* ctx) { ctx->GetVars<PluginsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<PluginsWindowContext>();

            auto plugin = mock_shared<MockPlugin>();
            ON_CALL(*plugin, name).WillByDefault(Return("Plugin Name"));
            ON_CALL(*plugin, path).WillByDefault(Return("test_path"));

            auto plugins = mock_shared<MockPlugins>();
            ON_CALL(*plugins, plugins).WillByDefault(Return(std::vector<std::weak_ptr<IPlugin>> { plugin }));

            auto shell = mock_shared<MockShell>();
            EXPECT_CALL(*shell, open(std::wstring(L"test_path"))).Times(1);

            context.ptr = register_test_module().with_plugins(plugins).with_shell(shell).build();;

            ctx->ItemClick("/**/Open##Plugin Name");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(shell.get()), true);
        });

    test<PluginsWindowContext>(engine, "Plugins Window", "Plugin Reloaded",
        [](ImGuiTestContext* ctx) { ctx->GetVars<PluginsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<PluginsWindowContext>();
            auto plugin = mock_shared<MockPlugin>();
            ON_CALL(*plugin, name).WillByDefault(Return("Plugin Name"));
            ON_CALL(*plugin, path).WillByDefault(Return("test_path"));
            EXPECT_CALL(*plugin, reload).Times(1);

            auto plugins = mock_shared<MockPlugins>();
            ON_CALL(*plugins, plugins).WillByDefault(Return(std::vector<std::weak_ptr<IPlugin>> { plugin }));

            context.ptr = register_test_module()
                .with_plugins(plugins)
                .build();

            ctx->ItemClick("/**/Reload##Plugin Name");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(plugin.get()), true);
        });
}
