#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.ui/Window.h>
#include <trview.ui/Checkbox.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(ViewOptions, BoundsCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    std::optional<bool> clicked;
    auto token = view_options.on_show_bounding_boxes += [&](bool value)
    {
        clicked = value;
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::show_bounding_boxes);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_TRUE(clicked.value());
}

TEST(ViewOptions, BoundsCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::show_bounding_boxes);
    ASSERT_FALSE(checkbox->state());
    
    view_options.set_show_bounding_boxes(true);
    ASSERT_TRUE(checkbox->state());
}
