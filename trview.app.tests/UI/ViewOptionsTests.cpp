#include <trview.app/UI/ViewOptions.h>
#include <trview.ui/Window.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(ViewOptions, HighlightCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::highlight);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::highlight);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, HighlightCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::highlight);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(ViewOptions::Names::highlight, true);
    ASSERT_TRUE(checkbox->state());
};

TEST(ViewOptions, TriggersCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::triggers);
    ASSERT_TRUE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::triggers);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, TriggersCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::triggers);
    ASSERT_TRUE(checkbox->state());

    view_options.set_toggle(ViewOptions::Names::triggers, false);
    ASSERT_FALSE(checkbox->state());
}

TEST(ViewOptions, HiddenGeometryCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::hidden_geometry);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::hidden_geometry);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, HiddenGeometryCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::hidden_geometry);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(ViewOptions::Names::hidden_geometry, true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, WaterCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::water);
    ASSERT_TRUE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::water);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, WaterCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::water);
    ASSERT_TRUE(checkbox->state());

    view_options.set_toggle(ViewOptions::Names::water, false);
    ASSERT_FALSE(checkbox->state());
}

TEST(ViewOptions, DepthCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::depth_enabled);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::depth_enabled);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, DepthCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::depth_enabled);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(ViewOptions::Names::depth_enabled, true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, WireframeCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::wireframe);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::wireframe);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, WireframeCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::wireframe);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(ViewOptions::Names::wireframe, true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, BoundsCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::show_bounding_boxes);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::show_bounding_boxes);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, BoundsCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::show_bounding_boxes);
    ASSERT_FALSE(checkbox->state());
    
    view_options.set_toggle(ViewOptions::Names::show_bounding_boxes, true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, FlipCheckboxToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::flip);
    ASSERT_FALSE(checkbox->state());
    checkbox->clicked({});
    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), ViewOptions::Names::flip);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, FlipCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::flip);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(ViewOptions::Names::flip, true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, FlipCheckboxEnabled)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::flip);
    ASSERT_TRUE(checkbox->enabled());

    view_options.set_flip_enabled(false);
    ASSERT_FALSE(checkbox->enabled());
}

TEST(ViewOptions, FlipFlagsToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::unordered_map<uint32_t, bool> raised;
    auto token = view_options.on_alternate_group += [&](uint32_t group, bool state)
    {
        raised[group] = state;
    };

    view_options.set_use_alternate_groups(true);
    view_options.set_alternate_groups({ 1, 3, 5 });

    auto group3 = window.find<ui::Button>(ViewOptions::Names::group + "3");
    ASSERT_NE(group3, nullptr);
    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOff);

    group3->clicked({});

    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOn);
    ASSERT_EQ(raised.size(), 1);
    ASSERT_EQ(raised[3], true);
}

TEST(ViewOptions, FlipFlagsUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    view_options.set_use_alternate_groups(true);
    view_options.set_alternate_groups({ 1, 3, 5 });

    auto group3 = window.find<ui::Button>(ViewOptions::Names::group + "3");
    ASSERT_NE(group3, nullptr);
    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOff);

    view_options.set_alternate_group(3, true);
    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOn);
}

TEST(ViewOptions, FlipCheckboxHiddenWithAlternateGroups)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(ViewOptions::Names::flip);
    ASSERT_TRUE(checkbox->visible());

    view_options.set_use_alternate_groups(true);
    ASSERT_FALSE(checkbox->visible(true));
}
