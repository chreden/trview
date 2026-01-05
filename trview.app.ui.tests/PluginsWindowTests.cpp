#include "pch.h"
#include "PluginsWindowTests.h"
#include <trview.app/Windows/Plugins/PluginsWindow.h>
#include <trview.app/Mocks/Plugins/IPlugin.h>
#include <trview.app/Mocks/Plugins/IPlugins.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>

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
            std::shared_ptr<IDialogs> dialogs{ mock_shared<MockDialogs>() };
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

            std::unique_ptr<PluginsWindow> build()
            {
                auto window = std::make_unique<PluginsWindow>(plugins, shell, dialogs, messaging);
                window->receive_message(trview::Message{ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(UserSettings {.randomizer_tools = false, .randomizer = {}}) });
                return window;
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

            test_module& with_dialogs(std::shared_ptr<IDialogs> dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_messaging(std::shared_ptr<IMessageSystem> messaging)
            {
                this->messaging = messaging;
                return *this;
            }
        };

        return test_module{};
    }

    struct PluginsWindowContext final
    {
        std::shared_ptr<PluginsWindow> ptr;
        std::shared_ptr<MockPlugins> plugins;

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

            context.plugins = mock_shared<MockPlugins>();
            ON_CALL(*context.plugins, plugins).WillByDefault(Return(std::vector<std::weak_ptr<IPlugin>> { plugin }));

            auto shell = mock_shared<MockShell>();
            EXPECT_CALL(*shell, open(std::wstring(L"test_path"))).Times(1);

            context.ptr = register_test_module().with_plugins(context.plugins).with_shell(shell).build();;

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

            context.plugins = mock_shared<MockPlugins>();
            ON_CALL(*context.plugins, plugins).WillByDefault(Return(std::vector<std::weak_ptr<IPlugin>> { plugin }));

            context.ptr = register_test_module()
                .with_plugins(context.plugins)
                .build();

            ctx->ItemClick("/**/Reload##Plugin Name");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(plugin.get()), true);
        });

    test<PluginsWindowContext>(engine, "Plugins Window", "Add Plugin Directories",
        [](ImGuiTestContext* ctx) { ctx->GetVars<PluginsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<PluginsWindowContext>();
            auto dialogs = mock_shared<MockDialogs>();
            ON_CALL(*dialogs, open_folder).WillByDefault(Return("test"));
            auto messaging = mock_shared<MockMessageSystem>();
            context.plugins = mock_shared<MockPlugins>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_plugins(context.plugins).with_messaging(messaging).build();

            trview::Message called_settings;
            EXPECT_CALL(*messaging, send_message).Times(AtLeast(1)).WillRepeatedly(SaveArg<0>(&called_settings));

            ctx->ItemClick("/**/+");

            const std::vector<std::string> expected{ "test" };
            IM_CHECK_EQ(std::static_pointer_cast<MessageData<UserSettings>>(called_settings.data)->value.plugin_directories, expected);
        });

    test<PluginsWindowContext>(engine, "Plugins Window", "Open Plugin Directories",
        [](ImGuiTestContext* ctx) { ctx->GetVars<PluginsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto shell = mock_shared<MockShell>();
            EXPECT_CALL(*shell, open(std::wstring(L"path"))).Times(1);

            UserSettings settings{ .plugin_directories = { "path" } };

            auto& context = ctx->GetVars<PluginsWindowContext>();
            context.plugins = mock_shared<MockPlugins>();
            context.ptr = register_test_module().with_shell(shell).with_plugins(context.plugins).build();
            context.ptr->receive_message(trview::Message{ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

            ctx->ItemClick("/**/Open##0");

            IM_CHECK_EQ(Mock::VerifyAndClear(shell.get()), true);
        });

    test<PluginsWindowContext>(engine, "Plugins Window", "Remove Plugin Directories",
        [](ImGuiTestContext* ctx) { ctx->GetVars<PluginsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<PluginsWindowContext>();
            auto messaging = mock_shared<MockMessageSystem>();
            context.plugins = mock_shared<MockPlugins>();
            context.ptr = register_test_module().with_plugins(context.plugins).with_messaging(messaging).build();
            

            UserSettings settings{ .plugin_directories = { "path" } };
            context.ptr->receive_message(trview::Message{ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

            trview::Message called_settings;
            EXPECT_CALL(*messaging, send_message).Times(AtLeast(1)).WillRepeatedly(SaveArg<0>(&called_settings));

            ctx->ItemClick("/**/Remove##0");

            const std::vector<std::string> expected;
            IM_CHECK_EQ(std::static_pointer_cast<MessageData<UserSettings>>(called_settings.data)->value.plugin_directories, expected);
        });
}
