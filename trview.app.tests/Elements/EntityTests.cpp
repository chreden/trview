#include <trview.app/Elements/Entity.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>

using namespace trlevel::mocks;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using testing::Return;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<trlevel::ILevel> level{ mock_shared<trlevel::mocks::MockLevel>() };
            IMesh::Source mesh_source = [](auto&&...) { return mock_shared<MockMesh>(); };
            std::shared_ptr<IMeshStorage> mesh_storage = mock_shared<MockMeshStorage>();
            trlevel::tr2_entity entity{};
            uint32_t index{ 0u };
            bool is_pickup{ false };
            std::string type{ "Lara" };
            std::vector<std::weak_ptr<ITrigger>> triggers;

            std::unique_ptr<Entity> build()
            {
                return std::make_unique<Entity>(mesh_source, *level, entity, *mesh_storage, index, type, triggers, is_pickup);
            }

            test_module& with_level(const std::shared_ptr<trlevel::ILevel>& level)
            {
                this->level = level;
                return *this;
            }

            test_module& with_pickup(bool value)
            {
                this->is_pickup = value;
                return *this;
            }

            test_module& with_entity(const trlevel::tr2_entity& entity)
            {
                this->entity = entity;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(Entity, OcbAdjustmentTrueForPickup)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb4));
    auto entity = register_test_module().with_level(level).with_pickup(true).build();
    ASSERT_TRUE(entity->needs_ocb_adjustment());
}

TEST(Entity, OcbAdjustmentFalseForNonPickup)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb4));
    auto entity = register_test_module().with_level(level).with_pickup(false).build();
    ASSERT_FALSE(entity->needs_ocb_adjustment());
}

TEST(Entity, OcbAdjustmentFalseForPickupWithNonMatchingOCB)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb4));
    trlevel::tr2_entity tr2_entity{};
    tr2_entity.Intensity2 = 1;
    auto entity = register_test_module().with_level(level).with_entity(tr2_entity).with_pickup(true).build();
    ASSERT_FALSE(entity->needs_ocb_adjustment());
}

TEST(Entity, OcbAdjustmentNotDonePreTR4)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb3));
    auto entity = register_test_module().with_level(level).with_pickup(true).build();
    ASSERT_FALSE(entity->needs_ocb_adjustment());
}
