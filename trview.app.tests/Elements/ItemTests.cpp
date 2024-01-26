#include <trview.app/Elements/Item.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.tests.common/Mocks.h>

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
            TypeInfo type{ .name = "Lara", .pickup = false };
            std::vector<std::weak_ptr<ITrigger>> triggers;
            std::shared_ptr<ILevel> owning_level{ mock_shared<MockLevel>() };
            std::shared_ptr<IRoom> room { mock_shared<MockRoom>() };

            std::unique_ptr<Item> build()
            {
                return std::make_unique<Item>(mesh_source, *level, entity, *mesh_storage, owning_level, index, type, triggers, room);
            }

            test_module& with_level(const std::shared_ptr<trlevel::ILevel>& level)
            {
                this->level = level;
                return *this;
            }

            test_module& with_pickup(bool value)
            {
                type.pickup = value;
                return *this;
            }

            test_module& with_entity(const trlevel::tr2_entity& entity)
            {
                this->entity = entity;
                return *this;
            }

            test_module& with_type_info(const TypeInfo& type_info)
            {
                this->type = type_info;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(Item, OcbAdjustmentTrueForPickup)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb4));
    auto entity = register_test_module().with_level(level).with_pickup(true).build();
    ASSERT_TRUE(entity->needs_ocb_adjustment());
}

TEST(Item, OcbAdjustmentFalseForNonPickup)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb4));
    auto entity = register_test_module().with_level(level).with_pickup(false).build();
    ASSERT_FALSE(entity->needs_ocb_adjustment());
}

TEST(Item, OcbAdjustmentFalseForPickupWithNonMatchingOCB)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb4));
    trlevel::tr2_entity tr2_entity{};
    tr2_entity.Intensity2 = 1;
    auto entity = register_test_module().with_level(level).with_entity(tr2_entity).with_pickup(true).build();
    ASSERT_FALSE(entity->needs_ocb_adjustment());
}

TEST(Item, OcbAdjustmentNotDonePreTR4)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_version).WillRepeatedly(Return(trlevel::LevelVersion::Tomb3));
    auto entity = register_test_module().with_level(level).with_pickup(true).build();
    ASSERT_FALSE(entity->needs_ocb_adjustment());
}


TEST(Item, IsMutantEgg)
{
    ASSERT_TRUE(is_mutant_egg(*mock_shared<MockItem>()->with_type_id(163)));
    ASSERT_TRUE(is_mutant_egg(*mock_shared<MockItem>()->with_type_id(181)));
    ASSERT_FALSE(is_mutant_egg(*mock_shared<MockItem>()->with_type_id(0)));
}

TEST(Item, IsMutantEggId)
{
    ASSERT_TRUE(is_mutant_egg(163));
    ASSERT_TRUE(is_mutant_egg(181));
    ASSERT_FALSE(is_mutant_egg(0));
}

TEST(Item, MutantEggContents)
{
    ASSERT_EQ(20, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(0)));
    ASSERT_EQ(21, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(1)));
    ASSERT_EQ(23, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(2)));
    ASSERT_EQ(34, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(4)));
    ASSERT_EQ(22, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(8)));
    ASSERT_EQ(20, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(1851)));
}

TEST(Item, MutantEggContentsFlags)
{
    ASSERT_EQ(20, mutant_egg_contents(0));
    ASSERT_EQ(21, mutant_egg_contents(1));
    ASSERT_EQ(23, mutant_egg_contents(2));
    ASSERT_EQ(34, mutant_egg_contents(4));
    ASSERT_EQ(22, mutant_egg_contents(8));
    ASSERT_EQ(20, mutant_egg_contents(1851));
}

TEST(Item, PickupCategoryMarksPickup)
{
    auto item = register_test_module().with_type_info({ .pickup = false, .categories = { "Pickup" } }).build();
    ASSERT_TRUE(item->is_pickup());
}
