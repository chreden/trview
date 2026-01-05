#include "pch.h"
#include "ConsoleTests.h"

#include <trview.tests.common/Mocks.h>
#include <trview.app/Windows/Console/Console.h>
#include <trview.app/Mocks/Plugins/IPlugins.h>
#include <trview.app/Mocks/Plugins/IPlugin.h>
#include <trview.app/Mocks/UI/IFonts.h>
#include <trview.common/Mocks/Windows/IDialogs.h>

using namespace testing;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    struct ConsoleContext
    {
        std::shared_ptr<Console> ptr;
    };

    void render(ConsoleContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_console_tests(ImGuiTestEngine* engine)
{
    test<ConsoleContext>(engine, "Console", "Clear",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ConsoleContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ConsoleContext>();
            auto plugins = mock_shared<MockPlugins>();
            auto plugin = mock_shared<MockPlugin>();
            ON_CALL(*plugin, name).WillByDefault(testing::Return("Default"));
            ON_CALL(*plugin, messages).WillByDefault(testing::Return("Hello"));
            EXPECT_CALL(*plugin, clear_messages).Times(1);

            ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));
            context.ptr = std::make_shared<Console>(mock_shared<MockDialogs>(), plugins, mock_shared<MockFonts>());

            ctx->Yield();
            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("/Console 0/TabBar/Default/##Log")->ID).c_str(), "Hello");

            ctx->ItemClick("/Console 0/##menubar/Edit");
            ctx->ItemClick("/##Menu_00/Clear");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(plugin.get()), true);
        });

    test<ConsoleContext>(engine, "Console", "Command Executed",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ConsoleContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ConsoleContext>();
            auto plugins = mock_shared<MockPlugins>();
            auto plugin = mock_shared<MockPlugin>();
            ON_CALL(*plugin, name).WillByDefault(testing::Return("Default"));
            EXPECT_CALL(*plugin, execute("Test command")).Times(1);
            ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));
            context.ptr = std::make_shared<Console>(mock_shared<MockDialogs>(), plugins, mock_shared<MockFonts>());

            ctx->ItemInputValue("/Console 0/TabBar/Default/##input", "Test command");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(plugin.get()), true);
        });

    test<ConsoleContext>(engine, "Console", "Command History",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ConsoleContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ConsoleContext>();
            auto plugins = mock_shared<MockPlugins>();
            auto plugin = mock_shared<MockPlugin>();
            ON_CALL(*plugin, name).WillByDefault(testing::Return("Default"));
            EXPECT_CALL(*plugin, execute("Test command")).Times(2);
            ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));
            context.ptr = std::make_shared<Console>(mock_shared<MockDialogs>(), plugins, mock_shared<MockFonts>());

            ctx->ItemInputValue("/Console 0/TabBar/Default/##input", "Test command");
            ctx->KeyPress(ImGuiKey_UpArrow);
            ctx->KeyPress(ImGuiKey_Enter);
            ctx->Yield();

            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("/Console 0/TabBar/Default/##input")->ID).c_str(), "");
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(plugin.get()), true);
        });

    test<ConsoleContext>(engine, "Console", "Open",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ConsoleContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ConsoleContext>();
            auto plugins = mock_shared<MockPlugins>();
            auto plugin = mock_shared<MockPlugin>();
            EXPECT_CALL(*plugin, do_file("test.lua")).Times(1);
            ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));
            auto dialogs = mock_shared<MockDialogs>();
            ON_CALL(*dialogs, open_file).WillByDefault(testing::Return(IDialogs::FileResult{ .filename = "test.lua" }));
            context.ptr = std::make_shared<Console>(dialogs, plugins, mock_shared<MockFonts>());

            ctx->ItemClick("/Console 0/##menubar/File");
            ctx->ItemClick("/##Menu_00/Open");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(plugin.get()), true);
        });

    test<ConsoleContext>(engine, "Console", "Open Recent",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ConsoleContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ConsoleContext>();
            auto plugins = mock_shared<MockPlugins>();
            auto plugin = mock_shared<MockPlugin>();
            EXPECT_CALL(*plugin, do_file("test.lua")).Times(2);
            ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));
            auto dialogs = mock_shared<MockDialogs>();
            ON_CALL(*dialogs, open_file).WillByDefault(testing::Return(IDialogs::FileResult{ .filename = "test.lua" }));
            context.ptr = std::make_shared<Console>(dialogs, plugins, mock_shared<MockFonts>());

            ctx->ItemClick("/Console 0/##menubar/File");
            ctx->ItemClick("/##Menu_00/Open");

            ctx->ItemClick("/Console 0/##menubar/File");
            ctx->ItemClick("/##Menu_00/Open Recent");
            ctx->ItemClick("/##Menu_01/test.lua");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(plugin.get()), true);
        });
}