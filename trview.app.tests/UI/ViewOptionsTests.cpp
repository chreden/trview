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
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::geometry));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::geometry);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, HiddenGeometryCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::geometry)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::geometry, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::geometry)) & ImGuiItemStatusFlags_Checked);
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
TEST(ViewOptions, ItemsCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::items));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::items);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, RoomsCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::rooms));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::rooms);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, RoomsCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::rooms)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::rooms, false);
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::rooms)) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, LightingCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::lighting));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::lighting);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, LightingCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::lighting)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::lighting, false);
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::lighting)) & ImGuiItemStatusFlags_Checked);
}


TEST(ViewOptions, NotesCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
        {
            clicked = { name, value };
        };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::notes));

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::notes);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, NotesCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::notes)) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::notes, false);
    imgui.render();
    ASSERT_FALSE(imgui.status_flags(imgui.id("View Options").push(ViewOptions::Names::flags).id(IViewer::Options::notes)) & ImGuiItemStatusFlags_Checked);
}