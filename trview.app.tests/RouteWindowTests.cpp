#include <trview.app/Windows/RouteWindow.h>
#include <trview.ui.render/Mocks/IRenderer.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.ui/Mocks/Input/IInput.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>

using namespace DirectX::SimpleMath;
using namespace testing;
using namespace trview;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::mocks;
using namespace trview::tests;
using namespace trview::ui::mocks;
using namespace trview::ui::render::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IDeviceWindow::Source device_window_source{ [](auto&&...) { return std::make_unique<MockDeviceWindow>(); } };
            ui::render::IRenderer::Source renderer_source{ [](auto&&...) { return std::make_unique<MockRenderer>(); } };
            ui::IInput::Source input_source{ [](auto&&...) { return std::make_unique<MockInput>(); } };
            Window parent{ create_test_window(L"RouteWindowTests") };
            std::shared_ptr<IClipboard> clipboard{ std::make_shared<MockClipboard>() };
            std::shared_ptr<IDialogs> dialogs{ std::make_shared<MockDialogs>() };
            std::shared_ptr<IFiles> files{ std::make_shared<MockFiles>() };

            test_module with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }

            std::unique_ptr<RouteWindow> build()
            {
                return std::make_unique<RouteWindow>(device_window_source, renderer_source, input_source,
                    parent, clipboard, dialogs, files);
            }
        };
        return test_module{};
    }
}

TEST(RouteWindow, WaypointRoomPositionCalculatedCorrectly)
{
    const Vector3 room_pos{ 102400, 204800, 307200 };
    const Vector3 waypoint_pos{ 130, 250, 325 };
    const Vector3 expected{ 30720, 51200, 25600 };

    auto window = register_test_module().build();

    RoomInfo info {};
    info.x = room_pos.x;
    info.yBottom = room_pos.y;
    info.z = room_pos.z;

    auto room = std::make_shared<MockRoom>();
    EXPECT_CALL(*room, info).WillRepeatedly(Return(info));

    auto [mesh_ptr, mesh] = create_mock<MockMesh>();
    Waypoint waypoint(&mesh, waypoint_pos, 0);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_rooms({ room });
    window->set_route(&route);

    window->select_waypoint(0);
    auto waypoint_stats = window->root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
    ASSERT_NE(waypoint_stats, nullptr);

    auto items = waypoint_stats->items();
    auto item_iter = std::find_if(items.begin(), items.end(), [](auto i) { return i.value(L"Name") == L"Room Position"; });
    ASSERT_NE(item_iter, items.end());

    auto value = item_iter->value(L"Value");
    value.erase(std::remove_if(value.begin(), value.end(),
        [](wchar_t c)
        {
            return std::isspace(static_cast<int>(c));
        }), value.end());
    std::replace(value.begin(), value.end(), L',', L' ');

    int32_t x, y, z;
    std::wstringstream stream(value);
    stream >> x >> y >> z;

    ASSERT_EQ(x, expected.x);
    ASSERT_EQ(y, expected.y);
    ASSERT_EQ(z, expected.z);
}

TEST(RouteWindow, PositionValuesCopiedToClipboard)
{
    auto clipboard = std::make_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write(An<const Window&>(), std::wstring(L"133120, 256000, 332800"))).Times(1);

    auto window = register_test_module().with_clipboard(clipboard).build();

    const Vector3 waypoint_pos{ 130, 250, 325 };
    auto [mesh_ptr, mesh] = create_mock<MockMesh>();
    Waypoint waypoint(&mesh, waypoint_pos, 0);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_route(&route);
    window->select_waypoint(0);

    auto waypoint_stats = window->root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
    ASSERT_NE(waypoint_stats, nullptr);

    auto row = waypoint_stats->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());
}

TEST(RouteWindow, RoomPositionValuesCopiedToClipboard)
{
    auto clipboard = std::make_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write(An<const Window&>(), std::wstring(L"133120, 256000, 332800"))).Times(1);

    auto window = register_test_module().with_clipboard(clipboard).build();

    const Vector3 waypoint_pos{ 130, 250, 325 };
    auto [mesh_ptr, mesh] = create_mock<MockMesh>();
    Waypoint waypoint(&mesh, waypoint_pos, 0);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_route(&route);
    window->select_waypoint(0);

    auto waypoint_stats = window->root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
    ASSERT_NE(waypoint_stats, nullptr);

    auto row = waypoint_stats->find<ui::Control>(ui::Listbox::Names::row_name_format + "3");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());
}

TEST(RouteWindow, AddingWaypointNotesMarksRouteUnsaved)
{
    const Vector3 waypoint_pos{ 130, 250, 325 };
    auto [mesh_ptr, mesh] = create_mock<MockMesh>();
    Waypoint waypoint(&mesh, waypoint_pos, 0);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    auto notes_area = window->root_control()->find<ui::TextArea>(RouteWindow::Names::notes_area);
    ASSERT_NE(notes_area, nullptr);

    notes_area->on_text_changed(L"Test");
}

TEST(RouteWindow, ClearSaveMarksRouteUnsaved)
{
    const Vector3 waypoint_pos{ 130, 250, 325 };
    auto [mesh_ptr, mesh] = create_mock<MockMesh>();
    Waypoint waypoint(&mesh, waypoint_pos, 0);
    waypoint.set_save_file({ 0, 1, 2 });

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    auto clear_save = window->root_control()->find<ui::Button>(RouteWindow::Names::clear_save);
    ASSERT_NE(clear_save, nullptr);

    clear_save->on_click();
}

TEST(RouteWindow, ExportRouteButtonRaisesEvent)
{
    FAIL();
}

TEST(RouteWindow, ImportRouteButtonRaisesEvent)
{
    FAIL();
}

TEST(RouteWindow, ExportSaveButtonSavesFile)
{
    FAIL();
}

TEST(RouteWindow, AttachSaveButtonLoadsSave)
{
    FAIL();
}
