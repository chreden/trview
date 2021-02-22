#include <trview.app/Windows/Viewer.h>
#include <trview.common/Windows/Shortcuts.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Geometry/IPicking.h>
#include <trview.app/Mocks/UI/IViewerUI.h>
#include <trview.input/Mocks/IMouse.h>

using testing::NiceMock;
using testing::Return;
using namespace trview;
using namespace trview::graphics;
using namespace trview::tests;
using namespace DirectX::SimpleMath;

namespace
{
    /// Simulates a context menu activation - 
    void activate_context_menu(
        mocks::MockPicking& picking,
        input::mocks::MockMouse& mouse,
        PickResult::Type type,
        uint32_t index)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = type;
        pick_result.index = index;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(input::IMouse::Button::Right);
    }
}

/// Tests that the on_select_item event from the UI is observed and forwarded.
TEST(Viewer, SelectItemRaisedForValidItem)
{
    auto window = create_test_window(L"ViewerTests");

    Device device;
    ShaderStorage shader_storage;
    Shortcuts shortcuts(window);
    Route route(device, shader_storage);

    auto ui_ptr = std::make_unique<mocks::MockViewerUI>();
    auto& ui = *ui_ptr;

    Item item(123, 0, 0, L"Test", 0, 0, {}, Vector3::Zero);
    mocks::MockLevel level;

    std::vector<Item> items_list{ item };
    EXPECT_CALL(level, items)
        .WillRepeatedly([&]() { return items_list; });

    Viewer viewer(window, device, shader_storage, std::move(ui_ptr), std::make_unique<mocks::MockPicking>(), std::make_unique<input::mocks::MockMouse>(), shortcuts, &route);
    viewer.open(&level);

    std::optional<Item> raised_item;
    auto token = viewer.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    ui.on_select_item(0);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 123);
}

TEST(Viewer, SelectItemNotRaisedForInvalidItem)
{
    auto window = create_test_window(L"ViewerTests");

    Device device;
    ShaderStorage shader_storage;
    Shortcuts shortcuts(window);
    Route route(device, shader_storage);

    auto ui_ptr = std::make_unique<mocks::MockViewerUI>();
    auto& ui = *ui_ptr;

    Viewer viewer(window, device, shader_storage, std::move(ui_ptr), std::make_unique<mocks::MockPicking>(), std::make_unique<input::mocks::MockMouse>(), shortcuts, &route);

    std::optional<Item> raised_item;
    auto token = viewer.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    ui.on_select_item(0);

    ASSERT_FALSE(raised_item.has_value());
}

TEST(Viewer, ItemVisibilityRaisedForValidItem)
{
    auto window = create_test_window(L"ViewerTests");

    Device device;
    ShaderStorage shader_storage;
    Shortcuts shortcuts(window);
    Route route(device, shader_storage);

    auto ui_ptr = std::make_unique<mocks::MockViewerUI>();
    auto& ui = *ui_ptr;

    Item item(123, 0, 0, L"Test", 0, 0, {}, Vector3::Zero);
    mocks::MockLevel level;

    std::vector<Item> items_list{ item };
    EXPECT_CALL(level, items)
        .WillRepeatedly([&]() { return items_list; });

    auto picking_ptr = std::make_unique<mocks::MockPicking>();
    auto& picking = *picking_ptr;

    auto mouse_ptr = std::make_unique<input::mocks::MockMouse>();
    auto& mouse = *mouse_ptr;

    Viewer viewer(window, device, shader_storage, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, &route);
    viewer.open(&level);

    std::optional<std::tuple<Item, bool>> raised_item;
    auto token = viewer.on_item_visibility += [&raised_item](const auto& item, auto visible) { raised_item = { item, visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 0);

    ui.on_hide();

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(std::get<0>(raised_item.value()).number(), 123);
    ASSERT_FALSE(std::get<1>(raised_item.value()));
}

