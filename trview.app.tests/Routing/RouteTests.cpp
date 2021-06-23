#include <trview.app/Routing/Route.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Geometry/IMesh.h>

using namespace trview;
using namespace trview::mocks;
using namespace DirectX::SimpleMath;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::unique_ptr<ISelectionRenderer> selection_renderer = std::make_unique<MockSelectionRenderer>();
            IMesh::Source mesh_source = [](auto&&...) { return std::make_shared<MockMesh>(); };

            std::unique_ptr<Route> build()
            {
                return std::make_unique<Route>(std::move(selection_renderer), mesh_source);
            }
        };
        return test_module{};
    }
}

TEST(Route, Add)
{
    auto route = register_test_module().build();
    route->add(Vector3(0, 1, 0), 10);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 1);
    auto waypoint = route->waypoint(0);
    ASSERT_EQ(waypoint.position(), Vector3(0, 1, 0));
    ASSERT_EQ(waypoint.room(), 10);
    ASSERT_EQ(waypoint.type(), Waypoint::Type::Position);
}

TEST(Route, AddSpecificType)
{
    auto route = register_test_module().build();
    route->add(Vector3(0, 1, 0), 10, Waypoint::Type::Trigger, 100);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 1);
    auto waypoint = route->waypoint(0);
    ASSERT_EQ(waypoint.position(), Vector3(0, 1, 0));
    ASSERT_EQ(waypoint.room(), 10);
    ASSERT_EQ(waypoint.type(), Waypoint::Type::Trigger);
    ASSERT_EQ(waypoint.index(), 100);
}

TEST(Route, Clear)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, 0);
    ASSERT_TRUE(route->is_unsaved());
    route->set_unsaved(false);
    route->clear();
    ASSERT_TRUE(route->is_unsaved());
}

TEST(Route, ClearAlreadyEmpty)
{
    auto route = register_test_module().build();
    route->clear();
    ASSERT_FALSE(route->is_unsaved());
}

TEST(Route, InsertAtPosition)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 1);
    route->set_unsaved(false);
    route->insert(Vector3(0, 1, 0), 2, 1);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 3);
    auto waypoint = route->waypoint(1);
    ASSERT_EQ(waypoint.position(), Vector3(0, 1, 0));
    ASSERT_EQ(waypoint.room(), 2);
    ASSERT_EQ(waypoint.type(), Waypoint::Type::Position);
}

TEST(Route, InsertSpecificTypeAtPosition)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 1);
    route->set_unsaved(false);
    route->insert(Vector3(0, 1, 0), 2, 1, Waypoint::Type::Entity, 100);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 3);
    auto waypoint = route->waypoint(1);
    ASSERT_EQ(waypoint.position(), Vector3(0, 1, 0));
    ASSERT_EQ(waypoint.room(), 2);
    ASSERT_EQ(waypoint.type(), Waypoint::Type::Entity);
    ASSERT_EQ(waypoint.index(), 100);
}

TEST(Route, Insert)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 1);
    route->set_unsaved(false);
    route->select_waypoint(1);
    auto index = route->insert(Vector3(0, 1, 0), 2);
    ASSERT_EQ(index, 2);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 3);
    auto waypoint = route->waypoint(2);
    ASSERT_EQ(waypoint.position(), Vector3(0, 1, 0));
    ASSERT_EQ(waypoint.room(), 2);
    ASSERT_EQ(waypoint.type(), Waypoint::Type::Position);
}

TEST(Route, IsUnsaved)
{
    auto route = register_test_module().build();
    ASSERT_FALSE(route->is_unsaved());
    route->set_unsaved(true);
    ASSERT_TRUE(route->is_unsaved());
}

TEST(Route, Remove)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, 0);
    ASSERT_EQ(route->waypoints(), 1);
    route->set_unsaved(false);
    route->remove(0);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 0);
}

TEST(Route, SetColour)
{
    auto route = register_test_module().build();
    route->set_colour(Colour::Red);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->colour(), Colour::Red);
}

TEST(Route, SelectedWaypoint)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 0);
    route->set_unsaved(false);
    route->select_waypoint(1);
    ASSERT_FALSE(route->is_unsaved());
    ASSERT_EQ(route->selected_waypoint(), 1);
}

TEST(Route, SelectedWaypointAdjustedByRemove)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 0);
    route->set_unsaved(false);
    route->select_waypoint(1);
    ASSERT_FALSE(route->is_unsaved());
    ASSERT_EQ(route->selected_waypoint(), 1);
    route->remove(1);
    ASSERT_EQ(route->selected_waypoint(), 0);
}
