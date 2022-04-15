#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Windows/IViewer.h>
#include "TestImgui.h"

using namespace trview;

TEST(ViewOptions, HighlightCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::highlight));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::highlight);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, HighlightCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::highlight)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::highlight, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::highlight)) & ImGuiItemStatusFlags_Checked);
};

TEST(ViewOptions, TriggersCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::triggers));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::triggers);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, TriggersCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::triggers)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::triggers, false);
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::triggers)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, HiddenGeometryCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::hidden_geometry));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::hidden_geometry);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, HiddenGeometryCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::hidden_geometry)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::hidden_geometry, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::hidden_geometry)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, WaterCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::water));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::water);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, WaterCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::water)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::water, false);
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::water)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, DepthCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::depth_enabled));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::depth_enabled);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, DepthCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::depth_enabled)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::depth_enabled, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::depth_enabled)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, WireframeCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::wireframe));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::wireframe);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, WireframeCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::wireframe)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::wireframe, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::wireframe)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, BoundsCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::show_bounding_boxes));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::show_bounding_boxes);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, BoundsCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::show_bounding_boxes)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::show_bounding_boxes, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::show_bounding_boxes)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, FlipCheckboxToggle)
{
    ViewOptions view_options;
    view_options.set_flip_enabled(true);

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::flip));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::flip);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, FlipCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::flip)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::flip, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::flip)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, FlipCheckboxEnabled)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.item_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::flip)) & ImGuiItemFlags_Disabled);

    view_options.set_flip_enabled(true);
    imgui.render();
    ASSERT_FALSE(imgui.item_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::flip)) & ImGuiItemFlags_Disabled);
}

TEST(ViewOptions, FlipFlagsToggle)
{
    ViewOptions view_options;

    std::unordered_map<uint32_t, bool> raised;
    auto token = view_options.on_alternate_group += [&](uint32_t group, bool state)
    {
        raised[group] = state;
    };

    view_options.set_use_alternate_groups(true);
    view_options.set_alternate_groups({ 1, 3, 5 });

    tests::TestImgui imgui([&]() { view_options.render(); });
    auto original = imgui.style_colour(imgui.id("View Options").id("3##3_flip"), ImGuiCol_Button);
    imgui.click_element(imgui.id("View Options").id("3##3_flip"));
    imgui.render();

    ASSERT_EQ(raised.size(), 1);
    ASSERT_EQ(raised[3], true);
    auto changed = imgui.style_colour(imgui.id("View Options").id("3##3_flip"), ImGuiCol_Button);
    ASSERT_NE(original, changed);
}

TEST(ViewOptions, FlipFlagsUpdated)
{
    ViewOptions view_options;
    view_options.set_use_alternate_groups(true);
    view_options.set_alternate_groups({ 1, 3, 5 });

    tests::TestImgui imgui([&]() { view_options.render(); });
    auto original = imgui.style_colour(imgui.id("View Options").id("3##3_flip"), ImGuiCol_Button);
    view_options.set_alternate_group(3, true);
    imgui.render();

    auto changed = imgui.style_colour(imgui.id("View Options").id("3##3_flip"), ImGuiCol_Button);
    ASSERT_NE(original, changed);
}

TEST(ViewOptions, FlipCheckboxHiddenWithAlternateGroups)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.element_present(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::flip)));

    view_options.set_use_alternate_groups(true);
    imgui.reset();
    imgui.render();
    ASSERT_FALSE(imgui.element_present(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::flip)));
}


TEST(ViewOptions, TRLECheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::trle_colours));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::trle_colours);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, TRLECheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::trle_colours)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::trle_colours, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::trle_colours)) & ImGuiItemStatusFlags_Checked);
}
