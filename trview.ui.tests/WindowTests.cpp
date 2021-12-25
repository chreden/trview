#include <trview.ui/Window.h>
#include <trview.ui/Layouts/GridLayout.h>
#include <trview.ui/JsonLoader.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(Window, LoadFromJson)
{
    const std::string json = "{\"type\":\"window\",\"name\":\"test\",\"size\":{\"width\":100,\"height\":200},\"position\":{\"x\":300,\"y\":400},\"visible\":false,\"background_colour\":\"red\",\"horizontal_alignment\":\"centre\",\"vertical_alignment\":\"centre\",\"layout\":{\"type\":\"grid\"},\"children\":[{\"type\":\"window\",\"name\":\"child\"}]}";
    auto control = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto window = dynamic_cast<ui::Window*>(control.get());
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(window->name(), "test");
    ASSERT_EQ(window->size(), Size(100, 200));
    ASSERT_EQ(window->position(), Point(300, 400));
    ASSERT_EQ(window->visible(), false);
    auto layout = window->layout();
    ASSERT_NE(layout, nullptr);
    ASSERT_NE(dynamic_cast<const GridLayout*>(layout), nullptr);
    ASSERT_EQ(window->background_colour(), Colour::Red);
    ASSERT_EQ(window->horizontal_alignment(), Align::Centre);
    ASSERT_EQ(window->vertical_alignment(), Align::Centre);
    auto child = window->find<Control>("child");
    ASSERT_NE(child, nullptr);
}

TEST(Window, BackgroundColour)
{
    ui::Window window(Size(), Colour::White);
    ASSERT_EQ(window.background_colour(), Colour::White);
    window.set_background_colour(Colour::Red);
    ASSERT_EQ(window.background_colour(), Colour::Red);
}