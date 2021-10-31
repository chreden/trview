#include <trview.app/Routing/Waypoint.h>
#include <trview.app/Mocks/Geometry/IMesh.h>

using namespace trview;
using namespace trview::mocks;
using namespace DirectX::SimpleMath;

TEST(Waypoint, ConstructorProperties)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3(1, 2, 3), Vector3::Down, 12, IWaypoint::Type::Trigger, 23, Colour::Red);
    ASSERT_EQ(waypoint.position(), Vector3(1, 2, 3));
    ASSERT_EQ(waypoint.normal(), Vector3::Down);
    ASSERT_EQ(waypoint.room(), 12);
    ASSERT_EQ(waypoint.type(), IWaypoint::Type::Trigger);
    ASSERT_EQ(waypoint.index(), 23);
}

TEST(Waypoint, Difficulty)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red);
    ASSERT_EQ(waypoint.difficulty(), "Easy");
    waypoint.set_difficulty("Medium");
    ASSERT_EQ(waypoint.difficulty(), "Medium");
}

TEST(Waypoint, EmptySave)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red);
    ASSERT_FALSE(waypoint.has_save());
    waypoint.set_save_file({});
    ASSERT_FALSE(waypoint.has_save());
}

TEST(Waypoint, IsItem)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red);
    ASSERT_FALSE(waypoint.is_item());
    waypoint.set_is_item(true);
    ASSERT_TRUE(waypoint.is_item());
}

TEST(Waypoint, Notes)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3(1, 2, 3), Vector3::Down, 12, IWaypoint::Type::Trigger, 23, Colour::Red);
    waypoint.set_notes(L"Test notes\nNew line");
    ASSERT_EQ(waypoint.notes(), L"Test notes\nNew line");
}

TEST(Waypoint, RequiresGlitch)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red);
    ASSERT_FALSE(waypoint.requires_glitch());
    waypoint.set_requires_glitch(true);
    ASSERT_TRUE(waypoint.requires_glitch());
}

TEST(Waypoint, SaveFile)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red);
    ASSERT_FALSE(waypoint.has_save());
    waypoint.set_save_file({ 0x1 });
    ASSERT_TRUE(waypoint.has_save());
    ASSERT_EQ(waypoint.save_file(), std::vector<uint8_t>{ 0x1 });
}

TEST(Waypoint, VehicleRequired)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red);
    ASSERT_FALSE(waypoint.vehicle_required());
    waypoint.set_vehicle_required(true);
    ASSERT_TRUE(waypoint.vehicle_required());
}

TEST(Waypoint, Visibility)
{
    Waypoint waypoint(std::make_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red);
    ASSERT_TRUE(waypoint.visible());
    waypoint.set_visible(false);
    ASSERT_FALSE(waypoint.visible());
    waypoint.set_visible(true);
    ASSERT_TRUE(waypoint.visible());
}
