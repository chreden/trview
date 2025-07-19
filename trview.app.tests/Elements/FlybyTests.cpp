#include <trview.app/Elements/Flyby/Flyby.h>
#include <trview.tests.common/Event.h>
#include <trview.app/Mocks/Elements/IFlybyNode.h>

using namespace trlevel;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace DirectX::SimpleMath;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IFlybyNode::Source flyby_node_source{ [](auto&&...) { return mock_shared<MockFlybyNode>(); } };
            IMesh::Source mesh_source{ [](auto&&...) { return mock_shared<MockMesh>(); } };
            std::shared_ptr<IMesh> mesh{ mock_shared<MockMesh>() };
            std::vector<tr4_flyby_camera> nodes;
            uint32_t number{ 0u };

            Flyby build()
            {
                return Flyby(flyby_node_source, mesh_source, mesh, nodes, number);
            }
        };

        return test_module{};
    }
}

TEST(Flyby, SetVisible)
{
    auto flyby = register_test_module().build();

    bool raised = false;
    auto token = flyby.on_changed += capture_called(raised);

    flyby.set_visible(true);

    ASSERT_TRUE(raised);
}
