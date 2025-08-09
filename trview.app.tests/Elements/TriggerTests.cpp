#include <trview.app/Elements/Trigger.h>
#include <trview.tests.common/Mocks.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ISector.h>

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
            std::shared_ptr<IRoom> room { mock_shared<MockRoom>() };
            uint16_t x{ 0 };
            uint16_t z{ 0 };
            TriggerInfo trigger_info{};
            trlevel::LevelVersion level_version{ trlevel::LevelVersion::Tomb3 };
            IMesh::Source mesh_source{ [](auto&&...) { return mock_shared<MockMesh>(); } };
            std::shared_ptr<trview::ILevel> level{ mock_shared<MockLevel>() };
            std::shared_ptr<ISector> sector{ mock_shared<MockSector>() };

            std::unique_ptr<Trigger> build()
            {
                return std::make_unique<Trigger>(number, room, x, z, trigger_info, level_version, level, sector, mesh_source);
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

TEST(Trigger, Colour)
{
    auto trigger = register_test_module().build();
    ASSERT_EQ(trigger->colour(), ITrigger::Trigger_Colour);
    trigger->set_colour(Colour::Yellow);
    ASSERT_EQ(trigger->colour(), Colour::Yellow);
    trigger->set_colour(std::nullopt);
    ASSERT_EQ(trigger->colour(), ITrigger::Trigger_Colour);
}

TEST(Trigger, VisibilityRaisesOnChanged)
{
    auto trigger = register_test_module().build();

    bool raised = false;
    auto token = trigger->on_changed += [&]() { raised = true; };
    trigger->set_visible(false);

    ASSERT_EQ(raised, true);
}
