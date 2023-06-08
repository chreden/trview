#include <trview.app/Routing/Waypoint.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.tests.common/Mocks.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/ILevel.h>

using namespace testing;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace DirectX::SimpleMath;

TEST(Waypoint, ConstructorProperties)
{
    Waypoint waypoint(mock_shared<MockMesh>(), Vector3(1, 2, 3), Vector3::Down, 12, IWaypoint::Type::Trigger, 23, Colour::Red, Colour::Green);
    ASSERT_EQ(waypoint.position(), Vector3(1, 2, 3));
    ASSERT_EQ(waypoint.normal(), Vector3::Down);
    ASSERT_EQ(waypoint.room(), 12);
    ASSERT_EQ(waypoint.type(), IWaypoint::Type::Trigger);
    ASSERT_EQ(waypoint.index(), 23);
}

TEST(Waypoint, EmptySave)
{
    Waypoint waypoint(mock_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red, Colour::Green);
    ASSERT_FALSE(waypoint.has_save());
    waypoint.set_save_file({});
    ASSERT_FALSE(waypoint.has_save());
}

TEST(Waypoint, Item)
{
    auto item = mock_shared<MockItem>();
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, item(23)).WillByDefault(Return(item));

    auto route = mock_shared<MockRoute>();
    ON_CALL(*route, level).WillByDefault(Return(level));

    Waypoint waypoint(mock_shared<MockMesh>(), Vector3(1, 2, 3), Vector3::Down, 12, IWaypoint::Type::Entity, 23, Colour::Red, Colour::Green);
    ASSERT_EQ(waypoint.item().lock(), nullptr);
    waypoint.set_route(route);
    ASSERT_EQ(waypoint.item().lock(), item);
}

TEST(Waypoint, Normal)
{
    Waypoint waypoint(mock_shared<MockMesh>(), Vector3(1, 2, 3), Vector3::Down, 12, IWaypoint::Type::Trigger, 23, Colour::Red, Colour::Green);
    ASSERT_EQ(Vector3::Down, waypoint.normal());
    waypoint.set_normal(Vector3::Up);
    ASSERT_EQ(Vector3::Up, waypoint.normal());
}

TEST(Waypoint, Notes)
{
    Waypoint waypoint(mock_shared<MockMesh>(), Vector3(1, 2, 3), Vector3::Down, 12, IWaypoint::Type::Trigger, 23, Colour::Red, Colour::Green);
    waypoint.set_notes("Test notes\nNew line");
    ASSERT_EQ(waypoint.notes(), "Test notes\nNew line");
}

TEST(Waypoint, RandomizerProperties)
{
    Waypoint waypoint(mock_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red, Colour::Green);
    auto existing = waypoint.randomizer_settings();
    ASSERT_TRUE(existing.empty());
    existing["test1"] = std::string("Test");
    waypoint.set_randomizer_settings(existing);
    auto updated = waypoint.randomizer_settings();
    ASSERT_EQ(existing.size(), 1);
    ASSERT_EQ(std::get<std::string>(updated["test1"]), "Test");
}

TEST(Waypoint, SaveFile)
{
    Waypoint waypoint(mock_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red, Colour::Green);
    ASSERT_FALSE(waypoint.has_save());
    waypoint.set_save_file({ 0x1 });
    ASSERT_TRUE(waypoint.has_save());
    ASSERT_EQ(waypoint.save_file(), std::vector<uint8_t>{ 0x1 });
}

TEST(Waypoint, Trigger)
{
    auto trigger = mock_shared<MockTrigger>();
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, trigger(23)).WillByDefault(Return(trigger));

    auto route = mock_shared<MockRoute>();
    ON_CALL(*route, level).WillByDefault(Return(level));

    Waypoint waypoint(mock_shared<MockMesh>(), Vector3(1, 2, 3), Vector3::Down, 12, IWaypoint::Type::Trigger, 23, Colour::Red, Colour::Green);
    ASSERT_EQ(waypoint.trigger().lock(), nullptr);
    waypoint.set_route(route);
    ASSERT_EQ(waypoint.trigger().lock(), trigger);
}

TEST(Waypoint, Visibility)
{
    Waypoint waypoint(mock_shared<MockMesh>(), Vector3::Zero, Vector3::Down, 0, IWaypoint::Type::Position, 0, Colour::Red, Colour::Green);
    ASSERT_TRUE(waypoint.visible());
    waypoint.set_visible(false);
    ASSERT_FALSE(waypoint.visible());
    waypoint.set_visible(true);
    ASSERT_TRUE(waypoint.visible());
}
