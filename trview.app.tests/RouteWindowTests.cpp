#include <trview.app/Windows/RouteWindow.h>
#include <trview.ui.render/Mocks/IRenderer.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Elements/ILevel.h>

using namespace DirectX::SimpleMath;
using namespace testing;
using namespace trview;
using namespace trview::graphics::mocks;
using namespace trview::mocks;
using namespace trview::tests;
using namespace trview::ui::render::mocks;

TEST(RouteWindow, WaypointRoomPositionCalculatedCorrectly)
{
    const Vector3 room_pos{ 102400, 204800, 307200 };
    const Vector3 waypoint_pos{ 130, 250, 325 };
    const Vector3 expected{ 30720, 51200, 25600 };

    auto [renderer_ptr_source, renderer] = create_mock<MockRenderer>();
    auto renderer_ptr = std::move(renderer_ptr_source);
    RouteWindow window([&](auto) { return std::make_unique<MockDeviceWindow>(); }, [&](auto) { return std::move(renderer_ptr); }, create_test_window(L"RouteWindowTests"));

    // All of these dependencies can be removed when room comes from DI (is IRoom)
    auto [trlevel_ptr, trlevel] = create_mock<trlevel::mocks::MockLevel>();
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto [texture_storage_ptr, texture_storage] = create_mock<MockLevelTextureStorage>();
    auto [mesh_storage_ptr, mesh_storage] = create_mock<MockMeshStorage>();
    trlevel::tr3_room tr_room{};
    tr_room.info.x = room_pos.x;
    tr_room.info.yBottom = room_pos.y;
    tr_room.info.z = room_pos.z;

    auto room = std::make_unique<Room>(
        [](auto, auto, auto, auto, auto) { return std::make_unique<MockMesh>(); },
        trlevel, tr_room, texture_storage, mesh_storage, 0, level);

    auto [mesh_ptr, mesh] = create_mock<MockMesh>();
    Waypoint waypoint(&mesh, waypoint_pos, 0);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window.set_rooms({ room.get() });
    window.set_route(&route);

    window.select_waypoint(0);
    auto waypoint_stats = window.root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
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