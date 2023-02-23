#include <trview.app/Elements/Trigger.h>
#include <trview.tests.common/Mocks.h>

using namespace trlevel;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            uint32_t number{ 0 };
            uint32_t room{ 0 };
            uint16_t x{ 0 };
            uint16_t z{ 0 };
            TriggerInfo trigger_info{};
            trlevel::LevelVersion level_version{ trlevel::LevelVersion::Tomb3 };
            IMesh::TransparentSource mesh_source{ [](auto&&...) { return mock_shared<MockMesh>(); } };
            std::shared_ptr<trview::ILevel> level{ mock_shared<MockLevel>() };

            std::unique_ptr<Trigger> build()
            {
                return std::make_unique<Trigger>(number, room, x, z, trigger_info, level_version, level, mesh_source);
            }

            test_module& with_info(const TriggerInfo& info)
            {
                trigger_info = info;
                return *this;
            }

            test_module& with_level_version(trlevel::LevelVersion version)
            {
                level_version = version;
                return *this;
            }
        };

        return test_module{};
    }
}

TEST(Trigger, TimerUnsignedInTR3)
{
    TriggerInfo info{};
    info.timer = 248;
    auto trigger = register_test_module().with_level_version(LevelVersion::Tomb3).with_info(info).build();
    ASSERT_EQ(trigger->timer(), 248);
}

TEST(Trigger, TimerSignedInTR4)
{
    TriggerInfo info{};
    info.timer = 248;
    auto trigger = register_test_module().with_level_version(LevelVersion::Tomb4).with_info(info).build();
    ASSERT_EQ(trigger->timer(), -8);
}
