#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.ui/Window.h>
#include <trview.ui/Checkbox.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(ViewOptions, HighlightCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    std::optional<bool> clicked;
    auto token = view_options.on_highlight += [&](bool value)
    {
        clicked = value;
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::highlight);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_TRUE(clicked.value());
}

TEST(ViewOptions, HighlightCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::highlight);
    ASSERT_FALSE(checkbox->state());

    view_options.set_highlight(true);
    ASSERT_TRUE(checkbox->state());
};

TEST(ViewOptions, TriggersCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    std::optional<bool> clicked;
    auto token = view_options.on_show_triggers += [&](bool value)
    {
        clicked = value;
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::triggers);
    ASSERT_TRUE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_FALSE(clicked.value());
}

TEST(ViewOptions, TriggersCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::triggers);
    ASSERT_TRUE(checkbox->state());

    view_options.set_show_triggers(false);
    ASSERT_FALSE(checkbox->state());
}

TEST(ViewOptions, HiddenGeometryCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    std::optional<bool> clicked;
    auto token = view_options.on_show_hidden_geometry += [&](bool value)
    {
        clicked = value;
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::hidden_geometry);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_TRUE(clicked.value());
}

TEST(ViewOptions, HiddenGeometryCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::hidden_geometry);
    ASSERT_FALSE(checkbox->state());

    view_options.set_show_hidden_geometry(true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, WaterCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    std::optional<bool> clicked;
    auto token = view_options.on_show_water += [&](bool value)
    {
        clicked = value;
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::water);
    ASSERT_TRUE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_FALSE(clicked.value());
}

TEST(ViewOptions, WaterCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::water);
    ASSERT_TRUE(checkbox->state());

    view_options.set_show_water(false);
    ASSERT_FALSE(checkbox->state());
}

TEST(ViewOptions, DepthCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    std::optional<bool> clicked;
    auto token = view_options.on_depth_enabled += [&](bool value)
    {
        clicked = value;
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::depth_enabled);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_TRUE(clicked.value());
}

TEST(ViewOptions, DepthCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::depth_enabled);
    ASSERT_FALSE(checkbox->state());

    view_options.set_depth_enabled(true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, WireframeCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    std::optional<bool> clicked;
    auto token = view_options.on_show_wireframe += [&](bool value)
    {
        clicked = value;
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::wireframe);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_TRUE(clicked.value());
}

TEST(ViewOptions, WireframeCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, MockLevelTextureStorage{});

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::wireframe);
    ASSERT_FALSE(checkbox->state());

    view_options.set_show_wireframe(true);
    ASSERT_TRUE(checkbox->state());
}

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

TEST(ViewOptions, FlipCheckboxToggle)
{
    FAIL();
}

TEST(ViewOptions, FlipCheckboxUpdated)
{
    FAIL();
}

TEST(ViewOptions, FlipCheckboxEnabled)
{
    FAIL();
}

TEST(ViewOptions, FlipFlagsToggle)
{
    FAIL();
}

TEST(ViewOptions, FlipFlagsUpdated)
{
    FAIL();
}
