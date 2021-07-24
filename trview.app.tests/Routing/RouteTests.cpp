#include <trview.app/Routing/Route.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>

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
            IWaypoint::Source waypoint_source = [](auto&&...) { return std::make_unique<MockWaypoint>(); };

            test_module& with_waypoint_source(IWaypoint::Source waypoint_source)
            {
                this->waypoint_source = waypoint_source;
                return *this;
            }

            std::unique_ptr<Route> build()
            {
                return std::make_unique<Route>(std::move(selection_renderer), mesh_source, waypoint_source);
            }
        };
        return test_module{};
    }

    struct WaypointDetails
    {
        Vector3 position;
        uint32_t room;
        IWaypoint::Type type;
        uint32_t index;
        Colour colour;
    };

    /// <summary>
    /// Get a waypoint source that makes mock waypoints and gives each one a sequential number.
    /// </summary>
    /// <param name="test_index">Initial number, should be 0 and live as long or longer than the source.</param>
    /// <returns>Waypoint source.</returns>
    IWaypoint::Source indexed_source(uint32_t test_index)
    {
        return [&](auto&& position, auto&& room, auto&& type, auto&& index, auto&& colour)
        {
            auto waypoint = std::make_unique<MockWaypoint>();
            waypoint->test_index = test_index++;
            return waypoint;
        };
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="route"></param>
    /// <returns></returns>
    std::vector<uint32_t> get_order(Route& route)
    {
        std::vector<uint32_t> waypoints;
        for (auto i = 0; i < route.waypoints(); ++i)
        {
            waypoints.push_back(static_cast<const MockWaypoint&>(route.waypoint(i)).test_index);
        }
        return waypoints;
    }
}

TEST(Route, Add)
{
    std::optional<WaypointDetails> waypoint_values;
    auto source = [&](auto&& position, auto&& room, auto&& type, auto&& index, auto&& colour)
    {
        waypoint_values = { position, room, type, index, colour };
        return std::make_unique<MockWaypoint>();
    };

    auto route = register_test_module().with_waypoint_source(source).build();
    route->add(Vector3(0, 1, 0), 10);

    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 1);
    ASSERT_TRUE(waypoint_values.has_value());
    ASSERT_EQ(waypoint_values.value().position, Vector3(0, 1, 0));
    ASSERT_EQ(waypoint_values.value().room, 10);
    ASSERT_EQ(waypoint_values.value().type, IWaypoint::Type::Position);
}

TEST(Route, AddSpecificType)
{
    std::optional<WaypointDetails> waypoint_values;
    auto source = [&](auto&& position, auto&& room, auto&& type, auto&& index, auto&& colour)
    {
        waypoint_values = { position, room, type, index, colour };
        return std::make_unique<MockWaypoint>();
    };
    auto route = register_test_module().with_waypoint_source(source).build();
    route->add(Vector3(0, 1, 0), 10, Waypoint::Type::Trigger, 100);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 1);
    ASSERT_EQ(waypoint_values.value().position, Vector3(0, 1, 0));
    ASSERT_EQ(waypoint_values.value().room, 10);
    ASSERT_EQ(waypoint_values.value().type, IWaypoint::Type::Trigger);
    ASSERT_EQ(waypoint_values.value().index, 100);
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
    uint32_t test_index = 0;
    auto route = register_test_module().with_waypoint_source(indexed_source(test_index)).build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 1);
    route->set_unsaved(false);
    route->insert(Vector3(0, 1, 0), 2, 1);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 3);

    const auto order = get_order(*route);
    const auto expected = std::vector<uint32_t>{ 0u, 2u, 1u };
    ASSERT_EQ(order, expected);
}

TEST(Route, InsertSpecificTypeAtPosition)
{
    uint32_t test_index = 0;
    auto route = register_test_module().with_waypoint_source(indexed_source(test_index)).build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 1);
    route->set_unsaved(false);
    route->insert(Vector3(0, 1, 0), 2, 1, Waypoint::Type::Entity, 100);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 3);
    auto& waypoint = route->waypoint(1);

    const auto order = get_order(*route);
    const auto expected = std::vector<uint32_t>{ 0u, 2u, 1u };
    ASSERT_EQ(order, expected);
}

TEST(Route, Insert)
{
    uint32_t test_index = 0;
    auto route = register_test_module().with_waypoint_source(indexed_source(test_index)).build();
    route->add(Vector3::Zero, 0);
    route->add(Vector3::Zero, 1);
    route->set_unsaved(false);
    route->select_waypoint(1);
    auto index = route->insert(Vector3(0, 1, 0), 2);
    ASSERT_EQ(index, 2);
    ASSERT_TRUE(route->is_unsaved());
    ASSERT_EQ(route->waypoints(), 3);
    
    const auto order = get_order(*route);
    const auto expected = std::vector<uint32_t>{ 0u, 1u, 2u };
    ASSERT_EQ(order, expected);
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
