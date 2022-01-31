#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.tests.common/Window.h>

#include <trview.graphics/Device.h>s

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_ENABLE_TEST_ENGINE
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/backends/imgui_impl_win32.h>
#include <external/imgui/backends/imgui_impl_dx11.h>

using namespace trview;
using namespace trview::mocks;

namespace
{
    class TestLogger
    {
    public:
        std::unordered_map<uint32_t, ImGuiItemStatusFlags> flags;
    };
}

void ImGuiTestEngineHook_ItemInfo(ImGuiContext* ctx, ImGuiID id, const char* label, ImGuiItemStatusFlags flags)
{
    TestLogger* logger = static_cast<TestLogger*>(ctx->TestEngine);
    if (!logger)
    {
        return;
    }
    logger->flags[id] = flags;
}

void ImGuiTestEngineHook_ItemAdd(ImGuiContext* ctx, const ImRect& bb, ImGuiID id)
{
}

void ImGuiTestEngineHook_Log(ImGuiContext* ctx, const char* fmt, ...)
{

}

const char* ImGuiTestEngine_FindItemDebugLabel(ImGuiContext* ctx, ImGuiID id)
{
    return "";
}

namespace
{
    ImGuiContext* setup_imgui(TestLogger& logger, const Window& window, const graphics::IDevice& device, const std::function<void()> render_callback)
    {
        auto context = ImGui::CreateContext();
        context->TestEngineHookItems = true;
        context->TestEngine = &logger;
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        if (io.BackendPlatformUserData == 0)
        {
            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX11_Init(device.device().Get(), device.context().Get());
            auto f = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 12.0f);
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        render_callback();
        ImGui::Render();

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        return context;
    }

    ImGuiID get_id(const std::string& window, const std::string& element)
    {
        auto context = ImGui::GetCurrentContext();
        ImGuiWindow* imgui_window = nullptr;
        for (const auto& w : context->Windows)
        {
            if (std::string(w->Name) == window)
            {
                context->CurrentWindow = w;
                ImGui::PushID("flags");
                auto x = w->GetID(element.c_str());
                ImGui::PopID();
                context->CurrentWindow = nullptr;
                return x;
            }
        }
        return 0;
    }

    void click_element(ImGuiContext* context, const std::string& window, std::unordered_map<std::string, std::tuple<uint32_t, Point>>& positions, const std::string& id)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuiWindow* imgui_window = nullptr;
        for (const auto& w : context->Windows)
        {
            if (std::string(w->Name) == window.c_str())
            {
                imgui_window = w;
                break;
            }
        }
        context->HoveredWindow = imgui_window;
        auto data = positions[id];
        context->HoveredId = std::get<0>(data);
        auto point = std::get<1>(data);
        context->IO.MousePos = ImVec2(point.x, point.y);
        context->IO.MouseClicked[0] = true;
    }

    std::function<void (const std::string&, uint32_t, const Point&)> track_element(const std::string& target)
    {
        return [target](const std::string& name, uint32_t id, const Point& pos)
        {
            if (name != target)
            {
                return;
            }
            auto context = ImGui::GetCurrentContext();
            context->HoveredId = id;
            context->IO.MousePos = ImVec2(pos.x, pos.y);
            context->IO.MouseClicked[0] = true;
        };
    }
}

TEST(ViewOptions, HighlightCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::highlight);
    click_element(context, "View Options", view_options._positions, IViewer::Options::highlight);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::highlight);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}
/*
TEST(ViewOptions, HighlightCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::highlight);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(IViewer::Options::highlight, true);
    ASSERT_TRUE(checkbox->state());
};
*/
TEST(ViewOptions, TriggersCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::triggers);
    click_element(context, "View Options", view_options._positions, IViewer::Options::triggers);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::triggers);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}
/*
TEST(ViewOptions, TriggersCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::triggers);
    ASSERT_TRUE(checkbox->state());

    view_options.set_toggle(IViewer::Options::triggers, false);
    ASSERT_FALSE(checkbox->state());
}*/

TEST(ViewOptions, HiddenGeometryCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::hidden_geometry);
    click_element(context, "View Options", view_options._positions, IViewer::Options::hidden_geometry);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::hidden_geometry);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}
/*
TEST(ViewOptions, HiddenGeometryCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::hidden_geometry);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(IViewer::Options::hidden_geometry, true);
    ASSERT_TRUE(checkbox->state());
}
*/
TEST(ViewOptions, WaterCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::water);
    click_element(context, "View Options", view_options._positions, IViewer::Options::water);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::water);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}
/*
TEST(ViewOptions, WaterCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::water);
    ASSERT_TRUE(checkbox->state());

    view_options.set_toggle(IViewer::Options::water, false);
    ASSERT_FALSE(checkbox->state());
}
*/
TEST(ViewOptions, DepthCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::depth_enabled);
    click_element(context, "View Options", view_options._positions, IViewer::Options::depth_enabled);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();


    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::depth_enabled);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}
/*
TEST(ViewOptions, DepthCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::depth_enabled);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(IViewer::Options::depth_enabled, true);
    ASSERT_TRUE(checkbox->state());
}
*/
TEST(ViewOptions, WireframeCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::wireframe);
    click_element(context, "View Options", view_options._positions, IViewer::Options::wireframe);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::wireframe);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}
/*
TEST(ViewOptions, WireframeCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::wireframe);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(IViewer::Options::wireframe, true);
    ASSERT_TRUE(checkbox->state());
}
*/
TEST(ViewOptions, BoundsCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::show_bounding_boxes);
    click_element(context, "View Options", view_options._positions, IViewer::Options::show_bounding_boxes);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::show_bounding_boxes);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, BoundsCheckboxUpdated)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    auto id = get_id("View Options", IViewer::Options::show_bounding_boxes);
    ASSERT_FALSE(logger.flags[id] & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::show_bounding_boxes, true);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    id = get_id("View Options", IViewer::Options::show_bounding_boxes);
    ASSERT_TRUE(logger.flags[id] & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, FlipCheckboxToggle)
{
    Window window = tests::create_test_window(L"Test");
    graphics::Device device;
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    TestLogger logger;
    auto context = setup_imgui(logger, window, device, [&]() { view_options.render(); });
    view_options._pos_fn = track_element(IViewer::Options::flip);
    click_element(context, "View Options", view_options._positions, IViewer::Options::flip);
    view_options.render();
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::flip);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}
/*
TEST(ViewOptions, FlipCheckboxUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::flip);
    ASSERT_FALSE(checkbox->state());

    view_options.set_toggle(IViewer::Options::flip, true);
    ASSERT_TRUE(checkbox->state());
}

TEST(ViewOptions, FlipCheckboxEnabled)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::flip);
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

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::flip);
    ASSERT_TRUE(checkbox->visible());

    view_options.set_use_alternate_groups(true);
    ASSERT_FALSE(checkbox->visible(true));
}
*/