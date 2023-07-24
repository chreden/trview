#include <trview.app/Routing/RandomizerRoute.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>
#include <trview.tests.common/Mocks.h>
#include <ranges>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace DirectX::SimpleMath;
using testing::Return;
using testing::NiceMock;
using testing::A;
using testing::SaveArg;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IWaypoint::Source waypoint_source = [](auto&&...) { return mock_unique<MockWaypoint>(); };
            std::shared_ptr<IRoute> route { mock_shared<MockRoute>() };

            test_module& with_route(const std::shared_ptr<IRoute>& route)
            {
                this->route = route;
                return *this;
            }

            test_module& with_waypoint_source(IWaypoint::Source waypoint_source)
            {
                this->waypoint_source = waypoint_source;
                return *this;
            }

            std::shared_ptr<RandomizerRoute> build()
            {
                return std::make_shared<RandomizerRoute>(route, waypoint_source);
            }
        };
        return test_module{};
    }
}

TEST(RandomizerRoute, AddForwardsToRoute)
{
    auto inner_route = mock_shared<MockRoute>();
    EXPECT_CALL(*inner_route, add(Vector3::Zero, Vector3::Down, 15));
    auto route = register_test_module()
        .with_route(inner_route)
        .build();
    route->add(Vector3::Zero, Vector3::Down, 15);
}

TEST(RandomizerRoute, ClearForwardsToRoute)
{
    auto inner_route = mock_shared<MockRoute>();
    EXPECT_CALL(*inner_route, clear);
    auto route = register_test_module()
        .with_route(inner_route)
        .build();
    route->clear();
}

TEST(RandomizerRoute, InsertForwardsToRoute)
{
    auto inner_route = mock_shared<MockRoute>();
    EXPECT_CALL(*inner_route, insert(Vector3::Zero, Vector3::Down, 15));
    auto route = register_test_module()
        .with_route(inner_route)
        .build();
    route->insert(Vector3::Zero, Vector3::Down, 15);
}

TEST(RandomizerRoute, Reload)
{
    const std::string contents = "{\n  \"level1\": [\n    {\n      \"X\": 0,\n      \"Y\": 0,\n      \"Z\": 0,\n      \"Room\": 0\n    }\n  ],\n  \"level2\": [\n    {\n      \"X\": 0,\n      \"Y\": 0,\n      \"Z\": 0,\n      \"Room\": 0\n    }\n  ]\n}";
    std::vector<uint8_t> bytes = contents 
        | std::views::transform([](const auto v) { return static_cast<uint8_t>(v); }) 
        | std::ranges::to<std::vector>();
    UserSettings settings{};
    auto files = mock_shared<MockFiles>();
    EXPECT_CALL(*files, load_file(A<const std::string&>())).WillRepeatedly(Return(bytes));

    auto route = register_test_module().build();
    
    route->set_filename("test.json");
    route->reload(files, settings);

    const std::vector<std::string> expected { "level1", "level2" };
    ASSERT_EQ(route->filenames(), expected);
}

TEST(RandomizerRoute, RemoveForwardsToRoute)
{
    std::shared_ptr<IWaypoint> waypoint = mock_shared<MockWaypoint>();
    auto inner_route = mock_shared<MockRoute>();
    EXPECT_CALL(*inner_route, remove(15));
    EXPECT_CALL(*inner_route, remove(waypoint));
    auto route = register_test_module()
        .with_route(inner_route)
        .build();
    route->remove(15);
    route->remove(waypoint);
}

TEST(RandomizerRoute, Save)
{
    auto route = register_test_module().build();
    auto files = mock_shared<MockFiles>();

    std::string contents;
    EXPECT_CALL(*files, save_file("test.json", A<const std::string&>()))
        .WillOnce(SaveArg<1>(&contents));

    UserSettings settings{};

    route->set_filename("test.json");
    route->add("level1", Vector3::Zero, Vector3::Down, 0);
    route->add("level2", Vector3::Zero, Vector3::Down, 0);
    route->save(files, settings);

    ASSERT_EQ(contents, "{\n  \"level1\": [\n    {\n      \"X\": 0,\n      \"Y\": 0,\n      \"Z\": 0,\n      \"Room\": 0\n    }\n  ],\n  \"level2\": [\n    {\n      \"X\": 0,\n      \"Y\": 0,\n      \"Z\": 0,\n      \"Room\": 0\n    }\n  ]\n}");
}

TEST(RandomizerRoute, SaveNoFileName)
{
    auto route = register_test_module().build();
    auto files = mock_shared<MockFiles>();

    EXPECT_CALL(*files, save_file(A<const std::string&>(), A<const std::string&>())).Times(0);

    UserSettings settings{};
    route->save(files, settings);
}

TEST(RandomizerRoute, SaveAs)
{
    auto route = register_test_module().build();
    auto files = mock_shared<MockFiles>();

    std::string contents;
    EXPECT_CALL(*files, save_file("test.json", A<const std::string&>()))
        .WillOnce(SaveArg<1>(&contents));

    UserSettings settings{};
    route->add("level1", Vector3::Zero, Vector3::Down, 0);
    route->add("level2", Vector3::Zero, Vector3::Down, 0);
    route->save_as(files, "test.json", settings);

    ASSERT_EQ(contents, "{\n  \"level1\": [\n    {\n      \"X\": 0,\n      \"Y\": 0,\n      \"Z\": 0,\n      \"Room\": 0\n    }\n  ],\n  \"level2\": [\n    {\n      \"X\": 0,\n      \"Y\": 0,\n      \"Z\": 0,\n      \"Room\": 0\n    }\n  ]\n}");
}

TEST(RandomizerRoute, SetLevelSwitchesWaypoints)
{
    std::shared_ptr<IWaypoint> waypoint = mock_shared<MockWaypoint>();
    const auto source = [=](auto&&...) { return waypoint; };

    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, filename).WillByDefault(Return("C:\\Temp\\test.tr2"));

    auto inner_route = mock_shared<MockRoute>();
    EXPECT_CALL(*inner_route, clear);
    EXPECT_CALL(*inner_route, add(waypoint));
    EXPECT_CALL(*inner_route, set_level);
    EXPECT_CALL(*inner_route, set_unsaved(false));
    auto route = register_test_module()
        .with_route(inner_route)
        .with_waypoint_source(source)
        .build();

    route->add("test.tr2", Vector3::Zero, Vector3::Down, 15);
    route->set_level(level);
}
