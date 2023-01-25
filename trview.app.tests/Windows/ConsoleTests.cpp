#include <trview.app/Windows/Console/Console.h>
#include "TestImgui.h"
#include <trview.common/Mocks/IFiles.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

TEST(Console, CommandEventRaised)
{
    Console console(mock_shared<MockFiles>(), mock_shared<MockDialogs>());

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
    Console console(mock_shared<MockFiles>(), mock_shared<MockDialogs>());

    TestImgui imgui([&]() { console.render(); });
    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::log)), "");

    console.print("Test log entry");
    imgui.render();
    ASSERT_EQ(imgui.item_text(imgui.id("Console 0").id(Console::Names::log)), "Test log entry");
}
