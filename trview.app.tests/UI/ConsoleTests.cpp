#include <trview.app/UI/Console.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(Console, CommandEventRaised)
{
    ui::Window window(Size(), Colour::White);
    Console console(window, JsonLoader(std::make_shared<MockShell>()));

    std::optional<std::wstring> raised;
    auto token = console.on_command += [&](auto value)
    {
        raised = value;
    };

    auto input = window.find<TextArea>(Console::Names::input);
    ASSERT_NE(input, nullptr);
    input->gained_focus();
    input->set_text(L"Test command");
    input->key_char(VK_RETURN);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), L"Test command");
    ASSERT_EQ(input->text(), L"");
}

TEST(Console, Visible)
{
    ui::Window window(Size(), Colour::White);
    Console console(window, JsonLoader(std::make_shared<MockShell>()));

    ASSERT_FALSE(console.visible());

    auto input = window.find<TextArea>(Console::Names::input);
    ASSERT_NE(input, nullptr);
    ASSERT_FALSE(input->visible(true));

    console.set_visible(true);
    ASSERT_TRUE(input->visible(true));
}

TEST(Console, PrintAddsLine)
{
    ui::Window window(Size(), Colour::White);
    Console console(window, JsonLoader(std::make_shared<MockShell>()));

    auto log = window.find<TextArea>(Console::Names::log);
    ASSERT_NE(log, nullptr);
    ASSERT_EQ(log->text(), L"");

    console.print(L"Test log entry");
    ASSERT_EQ(log->text(), L"Test log entry");
}
