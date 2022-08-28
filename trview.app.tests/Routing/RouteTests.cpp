#include <trview.app/Routing/Route.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>
#include <trview.app/Mocks/Camera/ICamera.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.tests.common/Mocks.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace DirectX::SimpleMath;
using testing::Return;
using testing::NiceMock;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::unique_ptr<ISelectionRenderer> selection_renderer = mock_unique<MockSelectionRenderer>();
            IWaypoint::Source waypoint_source = [](auto&&...) { return mock_unique<MockWaypoint>(); };
            UserSettings settings;

            test_module& with_selection_renderer(std::unique_ptr<ISelectionRenderer> selection_renderer)
            {
                this->selection_renderer = std::move(selection_renderer);
                return *this;
            }

            test_module& with_waypoint_source(IWaypoint::Source waypoint_source)
            {
                this->waypoint_source = waypoint_source;
                return *this;
            }

            std::unique_ptr<Route> build()
            {
                return std::make_unique<Route>(std::move(selection_renderer), waypoint_source, settings);
            }
        };
        return test_module{};
    }

    struct WaypointDetails
    {
        Vector3 position;
        Vector3 normal;
        uint32_t room;
        IWaypoint::Type type;
        uint32_t index;
        Colour colour;
        Colour stick_colour;
    };

    /// <summary>
    /// Get a waypoint source that makes mock waypoints and gives each one a sequential number.
    /// </summary>
    /// <param name="test_index">Initial number, should be 0 and live as long or longer than the source.</param>
    /// <returns>Waypoint source.</returns>
    IWaypoint::Source indexed_source(uint32_t& test_index)
    {
        return [&](auto&&...)
        {
            auto waypoint = mock_unique<MockWaypoint>();
            waypoint->test_index = test_index++;
            return waypoint;
        };
    }

    /// <summary>
    /// Get the test index values of the waypoints in the order they are in the list.
    /// </summary>
    /// <param name="route">The route to test.</param>
    /// <returns>The ordered index values.</returns>
    std::vector<uint32_t> get_order(Route& route)
    {
        std::vector<uint32_t> waypoints;
        for (auto i = 0u; i < route.waypoints(); ++i)
        {
            waypoints.push_back(static_cast<const MockWaypoint&>(route.waypoint(i)).test_index);
        }
        return waypoints;
    }
}

TEST(Route, Add)
{
    std::optional<WaypointDetails> waypoint_values;
    auto source = [&](auto&&... args)
    {
        waypoint_values = { args... };
        return mock_unique<MockWaypoint>();
    };

    auto route = register_test_module().with_waypoint_source(source).build();
    route->add(Vector3(0, 1, 0), Vector3::Down, 10);

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
    auto source = [&](auto&&... args)
    {
        waypoint_values = { args... };
        return mock_unique<MockWaypoint>();
    };
    auto route = register_test_module().with_waypoint_source(source).build();
    route->add(Vector3(0, 1, 0), Vector3::Down, 10, IWaypoint::Type::Trigger, 100);
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
    route->add(Vector3::Zero, Vector3::Down, 0);
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
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 1);
    route->set_unsaved(false);
    route->insert(Vector3(0, 1, 0), Vector3::Down, 2, 1);
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
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 1);
    route->set_unsaved(false);
    route->insert(Vector3(0, 1, 0), Vector3::Down, 2, 1, IWaypoint::Type::Entity, 100);
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
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 1);
    route->set_unsaved(false);
    route->select_waypoint(1);
    auto index = route->insert(Vector3(0, 1, 0), Vector3::Down, 2);
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

TEST(Route, MoveBackwards)
{
    uint32_t test_index = 0;
    auto route = register_test_module().with_waypoint_source(indexed_source(test_index)).build();
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 1);
    route->add(Vector3::Zero, Vector3::Down, 2);
    route->set_unsaved(false);
    route->move(2, 0);
    const auto order = get_order(*route);
    const auto expected = std::vector<uint32_t>{ 2u, 0u, 1u };
    ASSERT_EQ(order, expected);
    ASSERT_TRUE(route->is_unsaved());
}

TEST(Route, MoveForward)
{
    uint32_t test_index = 0;
    auto route = register_test_module().with_waypoint_source(indexed_source(test_index)).build();
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 1);
    route->add(Vector3::Zero, Vector3::Down, 2);
    route->set_unsaved(false);
    route->move(0, 2);
    const auto order = get_order(*route);
    const auto expected = std::vector<uint32_t>{ 1u, 2u, 0u };
    ASSERT_EQ(order, expected);
    ASSERT_TRUE(route->is_unsaved());
}

TEST(Route, Remove)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, Vector3::Down, 0);
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
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->set_unsaved(false);
    route->select_waypoint(1);
    ASSERT_FALSE(route->is_unsaved());
    ASSERT_EQ(route->selected_waypoint(), 1);
}

TEST(Route, SelectedWaypointAdjustedByRemove)
{
    auto route = register_test_module().build();
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->set_unsaved(false);
    route->select_waypoint(1);
    ASSERT_FALSE(route->is_unsaved());
    ASSERT_EQ(route->selected_waypoint(), 1);
    route->remove(1);
    ASSERT_EQ(route->selected_waypoint(), 0);
}

TEST(Route, Render)
{
    auto [selection_renderer_ptr, selection_renderer] = create_mock<MockSelectionRenderer>();
    EXPECT_CALL(selection_renderer, render).Times(1);

    auto [w1_ptr, w1] = create_mock<MockWaypoint>();
    auto [w2_ptr, w2] = create_mock<MockWaypoint>();
    EXPECT_CALL(w1, render).Times(1);
    EXPECT_CALL(w1, render_join).Times(1);
    EXPECT_CALL(w2, render).Times(1);
    EXPECT_CALL(w2, render_join).Times(0);

    auto w1_ptr_actual = std::move(w1_ptr);
    auto w2_ptr_actual = std::move(w2_ptr);
    auto source = [&](auto&&...)
    {
        return w1_ptr_actual ? std::move(w1_ptr_actual) : w2_ptr_actual ? std::move(w2_ptr_actual) : nullptr;
    };

    auto route = register_test_module().with_selection_renderer(std::move(selection_renderer_ptr)).with_waypoint_source(source).build();
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 0);
 
    NiceMock<MockCamera> camera;
    NiceMock<MockLevelTextureStorage> texture_storage;
    route->render(camera, texture_storage, true);
}

TEST(Route, RenderDoesNotJoinWhenRandoEnabled)
{
    auto [selection_renderer_ptr, selection_renderer] = create_mock<MockSelectionRenderer>();
    EXPECT_CALL(selection_renderer, render).Times(1);

    auto [w1_ptr, w1] = create_mock<MockWaypoint>();
    auto [w2_ptr, w2] = create_mock<MockWaypoint>();
    auto [w3_ptr, w3] = create_mock<MockWaypoint>();
    EXPECT_CALL(w1, render).Times(1);
    EXPECT_CALL(w1, render_join).Times(0);
    EXPECT_CALL(w2, render).Times(1);
    EXPECT_CALL(w2, type).WillRepeatedly(Return(IWaypoint::Type::Position));
    EXPECT_CALL(w2, render_join).Times(0);
    EXPECT_CALL(w3, render).Times(1);

    auto w1_ptr_actual = std::move(w1_ptr);
    auto w2_ptr_actual = std::move(w2_ptr);
    auto w3_ptr_actual = std::move(w3_ptr);
    auto source = [&](auto&&...) -> std::unique_ptr<IWaypoint>
    {
        if (w1_ptr_actual) { return std::move(w1_ptr_actual); }
        else if (w2_ptr_actual) { return std::move(w2_ptr_actual); }
        else if (w3_ptr_actual) { return std::move(w3_ptr_actual); }
        return nullptr;
    };

    auto route = register_test_module().with_selection_renderer(std::move(selection_renderer_ptr)).with_waypoint_source(source).build();
    route->set_randomizer_enabled(true);
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->add(Vector3::Zero, Vector3::Down, 0);

    NiceMock<MockCamera> camera;
    NiceMock<MockLevelTextureStorage> texture_storage;
    route->render(camera, texture_storage, true);
}

TEST(Route, RenderShowsSelection)
{
    auto [selection_renderer_ptr, selection_renderer] = create_mock<MockSelectionRenderer>();
    EXPECT_CALL(selection_renderer, render).Times(1);

    NiceMock<MockCamera> camera;
    NiceMock<MockLevelTextureStorage> texture_storage;

    auto route = register_test_module().with_selection_renderer(std::move(selection_renderer_ptr)).build();
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->render(camera, texture_storage, true);
}

TEST(Route, RenderDoesNotShowSelection)
{
    auto [selection_renderer_ptr, selection_renderer] = create_mock<MockSelectionRenderer>();
    EXPECT_CALL(selection_renderer, render).Times(0);

    NiceMock<MockCamera> camera;
    NiceMock<MockLevelTextureStorage> texture_storage;

    auto route = register_test_module().with_selection_renderer(std::move(selection_renderer_ptr)).build();
    route->add(Vector3::Zero, Vector3::Down, 0);
    route->render(camera, texture_storage, false);
}
