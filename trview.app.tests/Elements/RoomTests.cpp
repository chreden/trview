#include <trview.app/Elements/Room.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Camera/ICamera.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Geometry/ITransparencyBuffer.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/IStaticMesh.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.common/Algorithms.h>
#include <trview.common/Mocks/Logs/ILog.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using testing::Return;
using testing::NiceMock;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<ILevelTextureStorage> level_texture_storage{ mock_shared<MockLevelTextureStorage>() };
            std::shared_ptr<IMeshStorage> mesh_storage{ mock_shared<MockMeshStorage>() };
            IMesh::Source mesh_source{ [](auto&&...) { return mock_shared<MockMesh>(); } };
            std::shared_ptr<trlevel::ILevel> tr_level{ mock_shared<trlevel::mocks::MockLevel>() };
            trlevel::tr3_room room{ .alternate_group = 0 };
            uint32_t index{ 0u };
            std::shared_ptr<ILevel> level{ mock_shared<MockLevel>() };
            IStaticMesh::MeshSource static_mesh_source{ [](auto&&...) { return mock_shared<MockStaticMesh>(); } };
            IStaticMesh::PositionSource static_mesh_position_source{ [](auto&&...) { return mock_shared<MockStaticMesh>(); } };
            ISector::Source sector_source{ [](auto&&...) { return mock_shared<MockSector>(); } };
            std::shared_ptr<ILog> log{ mock_shared<MockLog>() };

            std::shared_ptr<Room> build()
            {
                auto new_room = std::make_shared<Room>(room, mesh_source, level_texture_storage, index, level);
                new_room->initialise(*tr_level, room, *mesh_storage, static_mesh_source, static_mesh_position_source, sector_source, 0, Activity(log, "Level", "Room 0"));
                return new_room;
            }

            test_module& with_level(const std::shared_ptr<ILevel>& level)
            {
                this->level = level;
                return *this;
            }

            test_module& with_number(uint32_t number)
            {
                this->index = number;
                return *this;
            }

            test_module& with_room(const trlevel::tr3_room& room)
            {
                this->room = room;
                return *this;
            }

            test_module& with_static_mesh_source(const IStaticMesh::MeshSource& static_mesh_source)
            {
                this->static_mesh_source = static_mesh_source;
                return *this;
            }

            test_module& with_static_mesh_position_source(const IStaticMesh::PositionSource& static_mesh_position_source)
            {
                this->static_mesh_position_source = static_mesh_position_source;
                return *this;
            }

            test_module& with_sector_source(const ISector::Source& sector_source)
            {
                this->sector_source = sector_source;
                return *this;
            }

            test_module& with_tr_level(const std::shared_ptr<trlevel::ILevel>& tr_level)
            {
                this->tr_level = tr_level;
                return *this;
            }

            test_module& with_log(const std::shared_ptr<ILog>& log)
            {
                this->log = log;
                return *this;
            }
        };
        return test_module{};
    }
}

/// <summary>
/// Tests that alternate group information is loaded from the level room.
/// </summary>
TEST(Room, AlternateGroupLoaded)
{
    trlevel::tr3_room level_room;
    level_room.alternate_group = 4;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->alternate_group(), 4);
}

/// <summary>
/// Tests that the room detects alternate room and has the correct alternate mode.
/// </summary>
TEST(Room, AlternateModeDetected)
{
    trlevel::tr3_room level_room;
    level_room.alternate_room = 100;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->alternate_mode(), IRoom::AlternateMode::HasAlternate);
    ASSERT_EQ(room->alternate_room(), 100);
}

/// <summary>
/// Tests that the logic embedded in TR5 alternate group numbers isn't hidden.
/// </summary>
TEST(Room, AlternateModeDetectedInvalidAlternate)
{
    trlevel::tr3_room level_room;
    level_room.alternate_group = 15;
    level_room.alternate_room = -1;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->alternate_mode(), IRoom::AlternateMode::HasAlternate);
    ASSERT_EQ(room->alternate_room(), -1);
    ASSERT_EQ(room->alternate_group(), 15);
}

/// <summary>
/// Tests that the bounding box is calculated.
/// </summary>
TEST(Room, BoundingBoxCalculated)
{
    trlevel::tr3_room level_room;
    level_room.num_x_sectors = 1;
    level_room.num_z_sectors = 1;
    level_room.info.yBottom = 0;
    level_room.info.yTop = -1024;
    auto room = register_test_module().with_room(level_room).build();
    auto box = room->bounding_box();
    ASSERT_EQ(box.Center.x, 0.5f);
    ASSERT_EQ(box.Center.z, 0.5f);
    ASSERT_EQ(box.Center.y, -0.5f);
}

/// <summary>
/// Tests that the bounding box takes into accounts entities.
/// </summary>
TEST(Room, BoundingBoxIncorporatesContents)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    trlevel::tr3_room level_room;
    level_room.num_x_sectors = 1;
    level_room.num_z_sectors = 1;
    level_room.info.yBottom = 0;
    level_room.info.yTop = -1024;
    auto room = register_test_module().with_room(level_room).build();

    auto entity = mock_shared<MockItem>();
    EXPECT_CALL(*entity, bounding_box).WillOnce(Return(BoundingBox(Vector3(1.0f, -1.0f, 1.0f), Vector3(0.5f, 0.5f, 0.5f))));
    room->add_entity(entity);
    room->update_bounding_box();

    auto box = room->bounding_box();
    ASSERT_EQ(box.Center.x, 0.75f);
    ASSERT_EQ(box.Center.z, 0.75f);
    ASSERT_EQ(box.Center.y, -0.75f);
}

/// <summary>
/// Tests that the centre is calculated correctly.
/// </summary>
TEST(Room, CentreCalculated)
{
    trlevel::tr3_room level_room;
    level_room.num_x_sectors = 1;
    level_room.num_z_sectors = 1;
    level_room.info.yBottom = 0;
    level_room.info.yTop = -1024;
    auto room = register_test_module().with_room(level_room).build();
    auto centre = room->centre();
    ASSERT_EQ(centre.x, 0.5f);
    ASSERT_EQ(centre.z, 0.5f);
    ASSERT_EQ(centre.y, -0.5f);
}

/// <summary>
/// Tests that the room gets transparent triangles when rendering.
/// </summary>
TEST(Room, GetTransparentTriangles)
{
    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    EXPECT_CALL(*entity, get_transparent_triangles).Times(1);
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, get_transparent_triangles).Times(1);
    room->add_entity(entity);
    room->add_trigger(trigger);
    NiceMock<MockTransparencyBuffer> transparency;
    NiceMock<MockCamera> camera;
    room->get_transparent_triangles(transparency, camera, IRoom::SelectionMode::NotSelected, RenderFilter::Default);
}

/// <summary>
/// Tests that the room gets transparent triangles when rendering, excluding items.
/// </summary>
TEST(Room, GetTransparentTrianglesWithoutItems)
{
    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    EXPECT_CALL(*entity, get_transparent_triangles).Times(0);
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, get_transparent_triangles).Times(1);
    room->add_entity(entity);
    room->add_trigger(trigger);
    NiceMock<MockTransparencyBuffer> transparency;
    NiceMock<MockCamera> camera;
    room->get_transparent_triangles(transparency, camera, IRoom::SelectionMode::NotSelected, set_flag(RenderFilter::Default, RenderFilter::Entities, false));
}

/// <summary>
/// Tests that the room gets transparent triangles when rendering, excluding triggers.
/// </summary>
TEST(Room, GetTransparentTrianglesWithoutTriggers)
{
    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    EXPECT_CALL(*entity, get_transparent_triangles).Times(1);
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, get_transparent_triangles).Times(0);
    room->add_entity(entity);
    room->add_trigger(trigger);
    NiceMock<MockTransparencyBuffer> transparency;
    NiceMock<MockCamera> camera;
    room->get_transparent_triangles(transparency, camera, IRoom::SelectionMode::NotSelected, set_flag(RenderFilter::Default, RenderFilter::Triggers, false));
}

/// <summary>
/// Tests that the room gets transparent triangles from its contents when rendering.
/// </summary>
TEST(Room, GetTransparentTrianglesFromContents)
{
    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    EXPECT_CALL(*entity, get_transparent_triangles).Times(1);
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, get_transparent_triangles).Times(0);
    room->add_entity(entity);
    room->add_trigger(trigger);
    NiceMock<MockTransparencyBuffer> transparency;
    NiceMock<MockCamera> camera;
    room->get_contained_transparent_triangles(transparency, camera, IRoom::SelectionMode::NotSelected, RenderFilter::Default);
}

/// <summary>
/// Tests that the info is loaded correctly from the room. Also checks that y is ignored and set to 0 as
/// it seems to be better to use yBottom and yTop.
/// </summary>
TEST(Room, InfoLoaded)
{
    trlevel::tr3_room level_room;
    level_room.info.x = 100;
    level_room.info.y = 200;
    level_room.info.z = 300;
    level_room.info.yBottom = 400;
    level_room.info.yTop = 500;
    auto room = register_test_module().with_room(level_room).build();
    auto info = room->info();
    ASSERT_EQ(info.x, 100);
    ASSERT_EQ(info.y, 0);
    ASSERT_EQ(info.z, 300);
    ASSERT_EQ(info.yBottom, 400);
    ASSERT_EQ(info.yTop, 500);
}

/// <summary>
/// Tests that setting an alternate room marks this room as an alternate and returns the correct alternate room.
/// </summary>
TEST(Room, IsAlternateSet)
{
    auto room = register_test_module().build();
    ASSERT_EQ(room->alternate_mode(), IRoom::AlternateMode::None);
    room->set_is_alternate(100);
    ASSERT_EQ(room->alternate_mode(), IRoom::AlternateMode::IsAlternate);
    ASSERT_EQ(room->alternate_room(), 100);
}

/// <summary>
/// Tests that neighbours are correctly extracted from the sectors in the room.
/// </summary>
TEST(Room, NeighboursLoaded)
{
    trlevel::tr3_room level_room;
    level_room.sector_list.resize(2);

    std::set<uint16_t> expected{ 0, 10, 20, 30 };
    auto sector1 = mock_shared<MockSector>();
    ON_CALL(*sector1, neighbours).WillByDefault(Return(std::set<uint16_t>{ 0, 20 }));
    auto sector2 = mock_shared<MockSector>();
    ON_CALL(*sector2, neighbours).WillByDefault(Return(std::set<uint16_t>{ 0, 10, 30 }));
    uint32_t times_called = 0;

    auto source = [&](auto&&...)
    {
        if (times_called++)
        {
            return sector2;
        }
        return sector1;
    };

    auto room = register_test_module().with_room(level_room).with_sector_source(source).build();
    auto actual = room->neighbours();
    ASSERT_EQ(times_called, 2);
    ASSERT_EQ(actual, expected);
}

/// <summary>
/// Tests that the room number is correctly returned.
/// </summary>
TEST(Room, NumberIsCorrect)
{
    auto room = register_test_module().with_number(100).build();
    ASSERT_EQ(room->number(), 100);
}

/// <summary>
/// Tests that the 'outside' flag is correctly detected.
/// </summary>
TEST(Room, OutsideDetected)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x8;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->outside(), true);
}

/// <summary>
/// Tests that the pick function tests entities when commanded.
/// </summary>
TEST(Room, PickTestsEntities)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    ON_CALL(*entity, visible).WillByDefault(Return(true));
    EXPECT_CALL(*entity, pick).Times(1).WillOnce(Return(PickResult{ .hit = true, .distance = 0, .position = {}, .centroid = {}, .type = PickResult::Type::Entity, .item = entity }));
    room->add_entity(entity);

    auto results = room->pick(Vector3(0, 0, -2), Vector3(0, 0, 1), PickFilter::Entities);
    ASSERT_EQ(results.size(), 1);
    auto result = results.front();
    ASSERT_EQ(result.hit, true);
    ASSERT_EQ(result.type, PickResult::Type::Entity);
    ASSERT_EQ(result.item.lock(), entity);
}

/// <summary>
/// Tests that the pick function tests triggers when commanded.
/// </summary>
TEST(Room, PickTestsTriggers)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    auto room = register_test_module().build();
    auto trigger = mock_shared<MockTrigger>();
    ON_CALL(*trigger, visible).WillByDefault(Return(true));
    EXPECT_CALL(*trigger, pick).Times(1).WillOnce(Return(PickResult{ .hit = true, .distance = 0, .position = {}, .centroid = {}, .type = PickResult::Type::Trigger, .trigger = trigger }));
    room->add_trigger(trigger);

    auto results = room->pick(Vector3(0, 0, -2), Vector3(0, 0, 1), PickFilter::Triggers);
    ASSERT_EQ(results.size(), 1);
    auto result = results.front();
    ASSERT_EQ(result.hit, true);
    ASSERT_EQ(result.type, PickResult::Type::Trigger);
    ASSERT_EQ(result.trigger.lock(), trigger);
}

/// <summary>
/// Tests that the pick function chooses the closest entry.
/// </summary>
TEST(Room, PickChoosesClosest)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    ON_CALL(*entity, visible).WillByDefault(Return(true));
    EXPECT_CALL(*entity, pick).Times(1).WillOnce(Return(PickResult{ .hit = true, .distance = 0.5f, .position = {}, .centroid = {}, .type = PickResult::Type::Entity, .item = entity }));
    room->add_entity(entity);

    auto entity2 = mock_shared<MockItem>();
    ON_CALL(*entity2, visible).WillByDefault(Return(true));
    EXPECT_CALL(*entity2, pick).Times(1).WillOnce(Return(PickResult{ .hit = true, .distance = 1.0f, .position = {}, .centroid = {}, .type = PickResult::Type::Entity, .item = entity2 }));
    room->add_entity(entity2);

    auto results = room->pick(Vector3(0, 0, -2), Vector3(0, 0, 1), PickFilter::Entities | PickFilter::Triggers);
    ASSERT_EQ(results.size(), 2);
    auto result = results.front();
    ASSERT_EQ(result.hit, true);
    ASSERT_EQ(result.type, PickResult::Type::Entity);
    ASSERT_EQ(result.item.lock(), entity);
    ASSERT_EQ(result.distance, 0.5f);
}

/// <summary>
/// Tests that the pick function chooses the closest entry.
/// </summary>
TEST(Room, PickChoosesEntityOverTrigger)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    ON_CALL(*entity, visible).WillByDefault(Return(true));
    EXPECT_CALL(*entity, pick).Times(1).WillOnce(Return(PickResult{ .hit = true, .distance = 1.0f, .position = {}, .centroid = {}, .type = PickResult::Type::Entity, .item = entity }));
    room->add_entity(entity);

    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, pick).Times(0);
    room->add_trigger(trigger);

    auto results = room->pick(Vector3(0, 0, -2), Vector3(0, 0, 1), PickFilter::Entities | PickFilter::Triggers);
    ASSERT_EQ(results.size(), 1);
    auto result = results.front();
    ASSERT_EQ(result.hit, true);
    ASSERT_EQ(result.type, PickResult::Type::Entity);
    ASSERT_EQ(result.item.lock(), entity);
    ASSERT_EQ(result.distance, 1.0f);
}

/// <summary>
/// Tests that the 'quicksand' flag is correctly detected when the version is >= TR3.
/// </summary>
TEST(Room, QuicksandDetectedAfterTR3)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x80;
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, version).WillByDefault(Return(trlevel::LevelVersion::Tomb3));
    auto room = register_test_module().with_room(level_room).with_level(level).build();
    ASSERT_EQ(room->quicksand(), true);
}

/// <summary>
/// Tests that the 'quicksand' flag is not detected before TR3.
/// </summary>
TEST(Room, QuicksandNotDetectedBeforeTR3)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x80;
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, version).WillByDefault(Return(trlevel::LevelVersion::Tomb1));
    auto room = register_test_module().with_room(level_room).with_level(level).build();
    ASSERT_EQ(room->quicksand(), false);
}

/// <summary>
/// Tests that entities are rendered when the room is rendered.
/// </summary>
TEST(Room, RendersContainedEntities)
{
    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    EXPECT_CALL(*entity, render).Times(1);
    room->add_entity(entity);
    room->render(NiceMock<MockCamera>{}, IRoom::SelectionMode::NotSelected, RenderFilter::Entities, {});
}

/// <summary>
/// Tests that entities are not rendered when the room is rendered and show items is false.
/// </summary>
TEST(Room, DoesNotRenderContainedEntitiesWhenShowItemsDisabled)
{
    auto room = register_test_module().build();
    auto entity = mock_shared<MockItem>();
    EXPECT_CALL(*entity, render).Times(0);
    room->add_entity(entity);
    room->render(NiceMock<MockCamera>{}, IRoom::SelectionMode::NotSelected, set_flag(RenderFilter::Default, RenderFilter::Entities, false), {});
}

/// <summary>
/// Tests that the number of X and Z sectors is correctly loaded.
/// </summary>
TEST(Room, SectorCountsLoaded)
{
    trlevel::tr3_room level_room;
    level_room.num_x_sectors = 10;
    level_room.num_z_sectors = 20;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->num_x_sectors(), 10);
    ASSERT_EQ(room->num_z_sectors(), 20);
}

/// <summary>
/// Tests that the room creates a sector for each sector in the room data and that they can be retrieved.
/// </summary>
TEST(Room, SectorsCreated)
{
    trlevel::tr3_room level_room;
    level_room.sector_list.resize(4);
    uint32_t times_called = 0;
    auto source = [&](auto&&...)
    {
        ++times_called;
        return mock_shared<MockSector>();
    };
    auto room = register_test_module().with_room(level_room).with_sector_source(source).build();
    ASSERT_EQ(times_called, 4);
    ASSERT_EQ(room->sectors().size(), 4);
}

/// <summary>
/// Tests that the appropriate number of static meshes in the room are created.
/// </summary>
TEST(Room, StaticMeshesLoaded)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    ON_CALL(*level, get_static_mesh).WillByDefault(testing::Return(trlevel::tr_staticmesh{}));

    trlevel::tr3_room level_room;
    level_room.static_meshes.resize(2);

    uint32_t times_called = 0;
    auto static_mesh_source = [&](auto&&...)
    {
        ++times_called;
        return mock_shared<MockStaticMesh>();
    };

    register_test_module().with_room(level_room).with_static_mesh_source(static_mesh_source).with_tr_level(level).build();
    ASSERT_EQ(times_called, 2);
}

/// <summary>
/// Tests that the appropriate number of sprites in the room are created.
/// </summary>
TEST(Room, SpritesLoaded)
{
    trlevel::tr3_room level_room;
    level_room.data.vertices.resize(1);
    level_room.data.sprites.resize(2);

    uint32_t times_called = 0;
    auto static_mesh_position_source = [&](auto&&...)
    {
        ++times_called;
        return mock_shared<MockStaticMesh>();
    };

    register_test_module().with_room(level_room).with_static_mesh_position_source(static_mesh_position_source).build();
    ASSERT_EQ(times_called, 2);
}

/// <summary>
/// Tests that a trigger can be retrieved by sector ID.
/// </summary>
TEST(Room, TriggerAtSectorId)
{
    trlevel::tr3_room level_room;
    level_room.num_x_sectors = 2;
    level_room.num_z_sectors = 3;
    auto room = register_test_module().with_room(level_room).build();

    auto trigger1 = mock_shared<MockTrigger>();
    ON_CALL(*trigger1, sector_id).WillByDefault(Return(0));
    auto trigger2 = mock_shared<MockTrigger>();
    ON_CALL(*trigger2, sector_id).WillByDefault(Return(4));

    room->add_trigger(trigger1);
    room->add_trigger(trigger2);

    auto t1 = room->trigger_at(0, 0);
    ASSERT_EQ(t1.lock(), trigger1);

    auto t2 = room->trigger_at(1, 1);
    ASSERT_EQ(t2.lock(), trigger2);
}

/// <summary>
/// Tests that a missing trigger return empty.
/// </summary>
TEST(Room, TriggerAtNotFound)
{
    auto room = register_test_module().build();
    auto trigger = room->trigger_at(1, 2);
    ASSERT_EQ(trigger.lock(), nullptr);
}

/// <summary>
/// Tests that the triggers have their geometries generated when called.
/// </summary>
TEST(Room, TriggerGeometryGenerated)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, set_triangles).Times(1);
    EXPECT_CALL(*trigger, set_position).Times(1);

    trlevel::tr3_room level_room;
    level_room.sector_list.resize(1);

    auto room = register_test_module().with_room(level_room).build();
    room->add_trigger(trigger);
    room->generate_trigger_geometry();
}

/// <summary>
/// Tests that the 'water' flag is correctly detected.
/// </summary>
TEST(Room, WaterDetected)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x1;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->water(), true);
}

TEST(Room, BoundingBoxesRendered)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    ON_CALL(*level, get_static_mesh).WillByDefault(testing::Return(trlevel::tr_staticmesh{}));
    auto mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*mesh, render_bounding_box).Times(1);
    trlevel::tr3_room level_room;
    level_room.static_meshes.push_back({});
    auto room = register_test_module()
        .with_room(level_room)
        .with_static_mesh_source([&](auto&&...) { return mesh; })
        .with_tr_level(level)
        .build();
    room->render_bounding_boxes(NiceMock<MockCamera>{});
}

TEST(Room, MissingStaticMeshesIgnored)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, get_static_mesh(0)).Times(1).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*level, get_static_mesh(1)).Times(1).WillOnce(Return(trlevel::tr_staticmesh{}));
    auto log = mock_shared<MockLog>();
    EXPECT_CALL(*log, log(Message::Status::Information, "Level", std::vector<std::string>{ "Room 0" }, testing::A<const std::string&>())).Times(2);
    EXPECT_CALL(*log, log(Message::Status::Error, "Level", std::vector<std::string>{ "Room 0" }, testing::A<const std::string&>())).Times(1);
    auto mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*mesh, render_bounding_box).Times(1);
    trlevel::tr3_room level_room;
    level_room.static_meshes.push_back({0, 0, 0, 0, 0, 0, 0});
    level_room.static_meshes.push_back({0, 0, 0, 0, 0, 0, 1});
    auto room = register_test_module()
        .with_room(level_room)
        .with_static_mesh_source([&](auto&&...) { return mesh; })
        .with_tr_level(level)
        .with_log(log)
        .build();
    room->render_bounding_boxes(NiceMock<MockCamera>{});
}

TEST(Room, RendersContainedCameraSinks)
{
    auto room = register_test_module().build();
    auto camera_sink = mock_shared<MockCameraSink>();
    EXPECT_CALL(*camera_sink, render).Times(1);
    room->add_camera_sink(camera_sink);
    room->render_camera_sinks(NiceMock<MockCamera>{});
}

TEST(Room, Sector)
{
    trlevel::tr3_room level_room;
    level_room.num_x_sectors = 2;
    level_room.num_z_sectors = 2;
    level_room.sector_list.resize(4);
    auto room = register_test_module().with_room(level_room).build();

    auto sector = room->sector(1, 1);
    auto s = sector.lock();
    ASSERT_TRUE(s);

    sector = room->sector(2, 2);
    s = sector.lock();
    ASSERT_FALSE(s);
}

TEST(Room, PickTestsStaticMesh)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    auto level = mock_shared<trlevel::mocks::MockLevel>();
    ON_CALL(*level, get_static_mesh).WillByDefault(testing::Return(trlevel::tr_staticmesh{}));

    trlevel::tr3_room level_room{ .static_meshes = { {} } };

    auto static_mesh = mock_shared<MockStaticMesh>();
    ON_CALL(*static_mesh, number).WillByDefault(Return(10));
    ON_CALL(*static_mesh, visible).WillByDefault(Return(true));
    EXPECT_CALL(*static_mesh, pick).Times(1).WillOnce(Return(PickResult{ .hit = true, .distance = 0, .position = {}, .centroid = {}, .type = PickResult::Type::StaticMesh, .static_mesh = static_mesh }));

    auto static_mesh_source = [&](auto&&...)
        {
            return static_mesh;
        };

    auto room = register_test_module()
        .with_room(level_room)
        .with_static_mesh_source(static_mesh_source)
        .with_tr_level(level)
        .build();

    auto results = room->pick(Vector3(0, 0, -2), Vector3(0, 0, 1), PickFilter::StaticMeshes);
    ASSERT_EQ(results.size(), 1);
    auto result = results.front();
    ASSERT_EQ(result.hit, true);
    ASSERT_EQ(result.type, PickResult::Type::StaticMesh);
    ASSERT_EQ(result.static_mesh.lock(), static_mesh);
}


TEST(Room, VisibilityRaisesOnChanged)
{
    auto room = register_test_module().build();

    bool raised = false;
    auto token = room->on_changed += [&]() { raised = true; };
    room->set_visible(false);

    ASSERT_EQ(raised, true);
}

