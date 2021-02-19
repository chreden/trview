#include <trview.app/Windows/Viewer.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/UI/IViewerUI.h>
#include <trview.common/Windows/Shortcuts.h>
#include <trview.app/Mocks/Elements/ILevel.h>

using testing::NiceMock;
using testing::Return;
using namespace trview;
using namespace trview::graphics;
using namespace trview::tests;
using namespace DirectX::SimpleMath;

TEST(Viewer, SelectItemRaised)
{
    auto window = create_test_window(L"ViewerTests");

    Device device;
    ShaderStorage shader_storage;
    Shortcuts shortcuts(window);
    Route route(device, shader_storage);

    std::unique_ptr<IViewerUI> ui_ptr = std::make_unique<mocks::MockViewerUI>();
    IViewerUI& ui = *ui_ptr;

    Item item(123, 0, 0, L"Test", 0, 0, {}, Vector3::Zero);
    mocks::MockLevel level;

    std::vector<Item> items_list{ item };
    EXPECT_CALL(level, items)
        .WillRepeatedly([&]() { return items_list; });

    Viewer viewer(window, device, shader_storage, std::move(ui_ptr), shortcuts, &route);
    viewer.open(&level);

    std::optional<Item> raised_item;
    auto token = viewer.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    ui.on_select_item(0);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 123);
}