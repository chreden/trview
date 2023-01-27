#include <trview.app/Windows/Console/Console.h>
#include "TestImgui.h"
#include <trview.common/Mocks/Windows/IDialogs.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

TEST(Console, CommandEventRaised)
{
    Console console(mock_shared<MockDialogs>());

    std::optional<std::string> raised;
    auto token = console.on_command += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { console.render(); });
    imgui.click_element(imgui.id("Console 0").id(Console::Names::input));
    imgui.enter_text("Test command");
    imgui.press_key(ImGuiKey_Enter);
    imgui.reset();
    imgui.render();

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), "Test command");
    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::input)), "");
}

TEST(Console, PrintAddsLine)
{
    Console console(mock_shared<MockDialogs>());

    TestImgui imgui([&]() { console.render(); });
    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::log)), "");

    console.print("Test log entry");
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::log)), "Test log entry");
}

TEST(Console, CommandHistory)
{
    Console console(mock_shared<MockDialogs>());

    std::vector<std::string> raised;
    auto token = console.on_command += [&](auto value)
    {
        raised.push_back(value);
    };

    TestImgui imgui([&]() { console.render(); });
    imgui.click_element(imgui.id("Console 0").id(Console::Names::input));
    imgui.enter_text("Test command");
    imgui.press_key(ImGuiKey_Enter);
    
    imgui.press_key(ImGuiKey_UpArrow);
    imgui.press_key(ImGuiKey_Enter);

    imgui.render();
    imgui.reset();
    imgui.render();

    ASSERT_EQ(raised.size(), 2);
    ASSERT_EQ(raised[0], "Test command");
    ASSERT_EQ(raised[1], "Test command");
    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::input)), "");
}

TEST(Console, Clear)
{
    Console console(mock_shared<MockDialogs>());

    console.print("Hello");

    TestImgui imgui([&]() { console.render(); });
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::log)), "Hello");

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("Edit"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("Edit"));
    imgui.hover_element(imgui.id("##Menu_00").id("Clear"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Clear"));

    imgui.reset();
    imgui.render();

    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::log)), "");
}

TEST(Console, Open)
{
    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, open_file).WillByDefault(testing::Return(IDialogs::FileResult{ "test.lua" }));

    Console console(dialogs);

    std::optional<std::string> raised;
    auto token = console.on_command += [&](const std::string& command)
    {
        raised = command;
    };

    TestImgui imgui([&]() { console.render(); });
    imgui.render();

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.hover_element(imgui.id("##Menu_00").id("Open"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Open"));

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "dofile(\"test.lua\")");
}

TEST(Console, OpenRecent)
{
    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, open_file).WillByDefault(testing::Return(IDialogs::FileResult{ "test.lua" }));

    Console console(dialogs);

    std::optional<std::string> raised;
    auto token = console.on_command += [&](const std::string& command)
    {
        raised = command;
    };

    TestImgui imgui([&]() { console.render(); });
    imgui.render();

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.hover_element(imgui.id("##Menu_00").id("Open"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Open"));

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "dofile(\"test.lua\")");
    raised.reset();

    imgui.hover_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.click_element(imgui.id("Console 0").push("##menubar").id("File"));
    imgui.hover_element(imgui.id("##Menu_00").id("Open Recent"));
    imgui.click_element_with_release(imgui.id("##Menu_00").id("Open Recent"));

    imgui.hover_element(imgui.id("##Menu_01").id("test.lua"));
    imgui.click_element_with_release(imgui.id("##Menu_01").id("test.lua"));

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), "dofile(\"test.lua\")");
}