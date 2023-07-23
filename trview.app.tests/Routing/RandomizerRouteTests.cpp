#include <trview.app/Routing/RandomizerRoute.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>
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

TEST(RandomizerRoute, AddUpdatesLevels)
{
    FAIL();
}

TEST(RandomizerRoute, AddForwardsToRoute)
{
    FAIL();
}

TEST(RandomizerRoute, ClearForwardsToRoute)
{
    FAIL();
}

TEST(RandomizerRoute, InsertForwardsToRoute)
{
    FAIL();
}

TEST(RandomizerRoute, Reload)
{
    FAIL();
}

TEST(RandomizerRoute, RemoveForwardsToRoute)
{
    FAIL();
}

TEST(RandomizerRoute, Save)
{
    FAIL();
}

TEST(RandomizerRoute, SaveAs)
{
    FAIL();
}

TEST(RandomizerRoute, SetLevelSwitchesWaypoints)
{
    FAIL();
}
