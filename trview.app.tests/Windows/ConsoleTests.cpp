#include <trview.app/Windows/Console/Console.h>
#include "TestImgui.h"
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.app/Mocks/Plugins/IPlugins.h>
#include <trview.app/Mocks/Plugins/IPlugin.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

TEST(Console, CommandExecuted)
{
    auto plugins = mock_shared<MockPlugins>();
    auto plugin = mock_shared<MockPlugin>();
    ON_CALL(*plugin, name).WillByDefault(testing::Return("Default"));
    EXPECT_CALL(*plugin, execute("Test command")).Times(1);
    ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));
    Console console(mock_shared<MockDialogs>(), plugins);

    TestImgui imgui([&]() { console.render(); });
    imgui.click_element(imgui.id("Console 0").push("TabBar").push("Default").id(Console::Names::input));
    imgui.enter_text("Test command");
    imgui.press_key(ImGuiKey_Enter);
    imgui.reset();
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").push("TabBar").push("Default").id(Console::Names::input)), "");
}

TEST(Console, CommandHistory)
{
    auto plugins = mock_shared<MockPlugins>();
    auto plugin = mock_shared<MockPlugin>();
    ON_CALL(*plugin, name).WillByDefault(testing::Return("Default"));
    EXPECT_CALL(*plugin, execute("Test command")).Times(2);
    ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));

    Console console(mock_shared<MockDialogs>(), plugins);

    TestImgui imgui([&]() { console.render(); });
    imgui.click_element(imgui.id("Console 0").push("TabBar").push("Default").id(Console::Names::input));
    imgui.enter_text("Test command");
    imgui.press_key(ImGuiKey_Enter);
    
    imgui.press_key(ImGuiKey_UpArrow);
    imgui.press_key(ImGuiKey_Enter);

    imgui.render();
    imgui.reset();
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").push("TabBar").push("Default").id(Console::Names::input)), "");
}

TEST(Console, Clear)
{
    auto plugins = mock_shared<MockPlugins>();
    auto plugin = mock_shared<MockPlugin>();
    ON_CALL(*plugin, name).WillByDefault(testing::Return("Default"));
    ON_CALL(*plugin, messages).WillByDefault(testing::Return("Hello"));
    EXPECT_CALL(*plugin, clear_messages).Times(1);

    ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));
    Console console(mock_shared<MockDialogs>(), plugins);

    TestImgui imgui([&]() { console.render(); });
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").push("TabBar").push("Default").id(Console::Names::log)), "Hello");

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("Edit"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("Edit"));
    imgui.hover_element(imgui.id("##Menu_00").id("Clear"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Clear"));

    imgui.reset();
    imgui.render();
}

TEST(Console, Open)
{
    auto plugins = mock_shared<MockPlugins>();
    auto plugin = mock_shared<MockPlugin>();
    EXPECT_CALL(*plugin, do_file("test.lua")).Times(1);
    ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));

    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, open_file).WillByDefault(testing::Return(IDialogs::FileResult{ "test.lua" }));

    Console console(dialogs, plugins);

    TestImgui imgui([&]() { console.render(); });
    imgui.render();

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.hover_element(imgui.id("##Menu_00").id("Open"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Open"));
}

TEST(Console, OpenRecent)
{
    auto plugins = mock_shared<MockPlugins>();
    auto plugin = mock_shared<MockPlugin>();
    EXPECT_CALL(*plugin, do_file("test.lua")).Times(2);
    ON_CALL(*plugins, plugins).WillByDefault(testing::Return(std::vector<std::weak_ptr<IPlugin>>{ plugin }));

    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, open_file).WillByDefault(testing::Return(IDialogs::FileResult{ "test.lua" }));

    Console console(dialogs, plugins);

    TestImgui imgui([&]() { console.render(); });
    imgui.render();

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.hover_element(imgui.id("##Menu_00").id("Open"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Open"));

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.hover_element(imgui.id("##Menu_00").id("Open Recent"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Open Recent"));

    imgui.hover_element(imgui.id("##Menu_01").id("test.lua"));
    imgui.click_element_with_release(imgui.id("##Menu_01").id("test.lua"));
}
