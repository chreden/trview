#include <trview.app/Windows/ItemsWindow.h>
#include <trview.tests.common/Window.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.ui/Button.h>
#include <trview.graphics/IFont.h>
#include <trview.common/Size.h>

using namespace trview;
using namespace trview::tests;
using testing::NiceMock;
using testing::Return;
using testing::ByMove;

class MockFontFactory : public graphics::IFontFactory
{
public:
    MOCK_METHOD2(store, void(const std::string&, const std::shared_ptr<DirectX::SpriteFont>&));
    MOCK_CONST_METHOD4(create_font, std::unique_ptr<graphics::IFont> (const std::string&, int, graphics::TextAlignment, graphics::ParagraphAlignment));
};

class MockFont : public graphics::IFont
{
public:
    MOCK_METHOD5(render, void (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&, const std::wstring&, float, float, const Colour&));
    MOCK_METHOD7(render, void (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&, const std::wstring&, float, float, float, float, const Colour&));
    MOCK_CONST_METHOD1(measure, Size (const std::wstring&));
    MOCK_CONST_METHOD1(is_valid_character, bool(wchar_t));
};

TEST(ItemsWindow, AddToRouteEventRaised)
{
    MockFontFactory font_factory;
    auto font = std::make_unique<MockFont>();

    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    graphics::Device device;
    graphics::ShaderStorage shader_storage;

    ItemsWindow window(device, shader_storage, font_factory, create_test_window(L"ItemsWindowTests"));

    std::optional<Item> raised_item;
    auto token = window.on_add_to_route += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window.set_items(items);

    auto button = window.root_control()->find<ui::Button>(ItemsWindow::Names::add_to_route_button);
    ASSERT_NE(button, nullptr);
    button->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(ItemsWindow, ClearSelectedItemClearsSelection)
{
    FAIL();
}

TEST(ItemsWindow, HideEventRaised)
{
    FAIL();
}

TEST(ItemsWindow, ItemSelectedNotRaisedWhenSyncItemDisabled)
{
    FAIL();
}

TEST(ItemsWindow, ItemSelectedRaisedWhenSyncItemEnabled)
{
    FAIL();
}

TEST(ItemsWindow, ItemVisibilityRaised)
{
    FAIL();
}

TEST(ItemsWindow, ItemsListFilteredWhenRoomSet)
{
    FAIL();
}

TEST(ItemsWindow, ItemsListPopulatedOnSet)
{
    FAIL();
}

TEST(ItemsWindow, ItemsListUpdatedWhenFiltered)
{
    FAIL();
}

TEST(ItemsWindow, ItemsListUpdatedWhenNotFiltered)
{
    FAIL();
}

TEST(ItemsWindow, SelectionSurvivesFiltering)
{
    FAIL();
}

TEST(ItemsWindow, TriggersLoadedForItem)
{
    FAIL();
}
