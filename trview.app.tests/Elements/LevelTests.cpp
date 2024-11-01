#include <trview.app/Elements/Level.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.graphics/mocks/IShaderStorage.h>
#include <trview.app/Mocks/Geometry/ITransparencyBuffer.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/ISoundSource.h>
#include <trview.app/Mocks/Camera/ICamera.h>
#include <trview.app/Mocks/Sound/ISoundStorage.h>
#include <trview.graphics/mocks/D3D/ID3D11DeviceContext.h>
#include <trview.graphics/mocks/IShader.h>
#include <trview.common/Algorithms.h>
#include <trview.common/Mocks/Logs/ILog.h>
#include <trview.graphics/mocks/IBuffer.h>
#include <trview.tests.common/Event.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trlevel;
using namespace trlevel::mocks;
using namespace trview::tests;
using testing::Return;
using testing::A;
using testing::NiceMock;
using namespace DirectX::SimpleMath;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IDevice> device{ mock_shared<MockDevice>() };
            std::shared_ptr<graphics::IShaderStorage> shader_storage{ mock_shared<MockShaderStorage>() };
            std::unique_ptr<trlevel::ILevel> level{ mock_unique<trlevel::mocks::MockLevel>() };
            std::shared_ptr<ILevelTextureStorage> level_texture_storage{ mock_shared<MockLevelTextureStorage>() };
            std::unique_ptr<IMeshStorage> mesh_storage{ mock_unique<MockMeshStorage>() };
            std::unique_ptr<ITransparencyBuffer> transparency_buffer{ mock_unique<MockTransparencyBuffer>() };
            std::unique_ptr<ISelectionRenderer> selection_renderer{ mock_unique<MockSelectionRenderer>() };
            IItem::EntitySource entity_source{ [](auto&&...) { return mock_shared<MockItem>(); } };
            IItem::AiSource ai_source{ [](auto&&...) { return mock_shared<MockItem>(); } };
            IRoom::Source room_source{ [](auto&&...) { return mock_shared<MockRoom>(); } };
            ITrigger::Source trigger_source{ [](auto&&...) {return mock_shared<MockTrigger>(); } };
            ILight::Source light_source{ [](auto&&...) { return std::make_shared<MockLight>(); } };
            std::shared_ptr<ILog> log{ mock_shared<MockLog>() };
            graphics::IBuffer::ConstantSource buffer_source{ [](auto&&...) { return mock_unique<MockBuffer>(); } };
            ICameraSink::Source camera_sink_source{ [](auto&&...) { return mock_shared<MockCameraSink>(); } };
            trlevel::ILevel::LoadCallbacks callbacks;
            ISoundSource::Source sound_source_source{ [](auto&&...) { return mock_shared<MockSoundSource>(); } };
            std::shared_ptr<ISoundStorage> sound_storage{ mock_shared<MockSoundStorage>() };

            std::shared_ptr<Level> build()
            {
                auto new_level = std::make_shared<Level>(device, shader_storage, level_texture_storage, std::move(transparency_buffer), std::move(selection_renderer), log, buffer_source, sound_storage);
                new_level->initialise(std::move(level), std::move(mesh_storage), entity_source, ai_source, room_source, trigger_source, light_source, camera_sink_source, sound_source_source, callbacks);
                return new_level;
            }

            test_module& with_level(std::unique_ptr<trlevel::ILevel>&& level)
            {
                this->level = std::move(level);
                return *this;
            }

            test_module& with_entity_source(const IItem::EntitySource& entity_source)
            {
                this->entity_source = entity_source;
                return *this;
            }

            test_module& with_ai_source(const IItem::AiSource& ai_source)
            {
                this->ai_source = ai_source;
                return *this;
            }

            test_module& with_room_source(const IRoom::Source& room_source)
            {
                this->room_source = room_source;
                return *this;
            }

            test_module& with_device(const std::shared_ptr<IDevice>& device)
            {
                this->device = device;
                return *this;
            }

            test_module& with_shader_storage(const std::shared_ptr<IShaderStorage>& shader_storage)
            {
                this->shader_storage = shader_storage;
                return *this;
            }

            test_module& with_light_source(const ILight::Source& light_source)
            {
                this->light_source = light_source;
                return *this;
            }

            test_module& with_trigger_source(const ITrigger::Source& trigger_source)
            {
                this->trigger_source = trigger_source;
                return *this;
            }
        };

        return test_module{};
    }
}

TEST(Level, LoadFromEntitySources)
{
    tr2_entity entity;
    entity.Room = 0;
    entity.TypeID = 123;

    tr4_ai_object ai_object{};

    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, get_version).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, num_entities()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, get_entity(0)).WillRepeatedly(Return(entity));
    EXPECT_CALL(mock_level, num_ai_objects()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, get_ai_object(0)).WillRepeatedly(Return(ai_object));

    uint32_t entity_source_called = 0;
    uint32_t ai_source_called = 0;

    auto level = register_test_module().with_level(std::move(mock_level_ptr))
        .with_entity_source(
            [&](auto&&...)
            {
                ++entity_source_called;
                return mock_shared<MockItem>();
            })
        .with_ai_source(
            [&](auto&&...)
            {
                ++ai_source_called;
                return mock_shared<MockItem>();
            }).build();

    ASSERT_EQ(entity_source_called, 1);
    ASSERT_EQ(ai_source_called, 1);
}

TEST(Level, LoadRooms)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, get_version).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(3));

    int room_called = 0;

    auto level = register_test_module().with_level(std::move(mock_level_ptr))
        .with_room_source(
            [&](auto&&...)
            {
                ++room_called;
                return mock_shared<MockRoom>();
            }).build();

    ASSERT_EQ(room_called, 3);
}

TEST(Level, OcbAdjustmentsPerformedWhenNeeded)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, num_entities()).WillRepeatedly(Return(1));
    int entity_source_called = 0;

    auto room = mock_shared<MockRoom>();
    PickResult result{};
    result.hit = true;
    EXPECT_CALL(*room, pick).WillRepeatedly(Return(std::vector<PickResult> { result }));
    
    auto level = register_test_module().with_level(std::move(mock_level_ptr))
        .with_room_source(
            [&](auto&&...)
            {
                return room;
            })
        .with_entity_source(
            [&](auto&&...)
            {
                ++entity_source_called;
                auto entity = mock_shared<MockItem>();
                EXPECT_CALL(*entity, needs_ocb_adjustment).WillRepeatedly(Return(true));
                EXPECT_CALL(*entity, adjust_y).Times(1);
                EXPECT_CALL(*entity, room).WillRepeatedly(Return(room));
                return entity;
            }).build();

    ASSERT_EQ(entity_source_called, 1);
}

TEST(Level, OcbAdjustmentsNotPerformedWhenNotNeeded)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, num_entities()).WillRepeatedly(Return(1));
    int entity_source_called = 0;
    auto level = register_test_module().with_level(std::move(mock_level_ptr))
        .with_room_source(
            [&](auto&&...)
            {
                auto room = mock_shared<MockRoom>();
                PickResult result{};
                result.hit = true;
                EXPECT_CALL(*room, pick).WillRepeatedly(Return(std::vector<PickResult> { result }));
                return room;
            })
        .with_entity_source(
            [&](auto&&...)
            {
                ++entity_source_called;
                auto entity = mock_shared<MockItem>();
                EXPECT_CALL(*entity, needs_ocb_adjustment).WillRepeatedly(Return(false));
                EXPECT_CALL(*entity, adjust_y).Times(0);
                return entity;
            }).build();

    ASSERT_EQ(entity_source_called, 1);
}

TEST(Level, PickUsesCorrectDefaultFilters)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));

    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));
    EXPECT_CALL(*room, pick(A<const Vector3&>(), A<const Vector3&>(), PickFilter::Geometry | PickFilter::Entities | PickFilter::StaticMeshes | PickFilter::Triggers)).Times(1);

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    NiceMock<MockCamera> camera;
    level->pick(camera, Vector3::Zero, Vector3::Forward);
}

TEST(Level, PickUsesCorrectOptionalFilters)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));

    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));
    EXPECT_CALL(*room, pick(A<const Vector3&>(), A<const Vector3&>(), PickFilter::Geometry | PickFilter::Entities | PickFilter::StaticMeshes | PickFilter::Triggers | PickFilter::AllGeometry | PickFilter::Lights)).Times(1);

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();
    level->set_show_triggers(true);
    level->set_show_geometry(true);
    level->set_show_lights(true);

    NiceMock<MockCamera> camera;
    level->pick(camera, Vector3::Zero, Vector3::Forward);
}

TEST(Level, PickUsesCorrectMinimalFilters)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));

    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));
    EXPECT_CALL(*room, pick(A<const Vector3&>(), A<const Vector3&>(), PickFilter::Geometry | PickFilter::StaticMeshes)).Times(1);

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();
    level->set_show_triggers(false);
    level->set_show_geometry(false);
    level->set_show_items(false);

    NiceMock<MockCamera> camera;
    level->pick(camera, Vector3::Zero, Vector3::Forward);
}

TEST(Level, BoundingBoxesNotRenderedWhenDisabled)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));

    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    NiceMock<MockShader> shader;
    auto shader_storage = mock_shared<MockShaderStorage>();
    EXPECT_CALL(*shader_storage, get).WillRepeatedly(Return(&shader));

    EXPECT_CALL(*room, render).Times(1);
    EXPECT_CALL(*room, render_bounding_boxes).Times(0);

    auto level = register_test_module()
        .with_device(device)
        .with_shader_storage(shader_storage)
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    NiceMock<MockCamera> camera;
    level->render(camera, false);
}

TEST(Level, BoundingBoxesRenderedWhenEnabled)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));

    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    NiceMock<MockShader> shader;
    auto shader_storage = mock_shared<MockShaderStorage>();
    EXPECT_CALL(*shader_storage, get).WillRepeatedly(Return(&shader));

    EXPECT_CALL(*room, render).Times(1);
    EXPECT_CALL(*room, render_bounding_boxes).Times(1);

    auto level = register_test_module()
        .with_device(device)
        .with_shader_storage(shader_storage)
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    level->set_show_bounding_boxes(true);

    NiceMock<MockCamera> camera;
    level->render(camera, false);
}

TEST(Level, SetShowBoundingBoxesRaisesLevelChangedEvent)
{
    auto level = register_test_module().build();

    uint32_t times_called = 0;
    auto token = level->on_level_changed += [&](auto&&...) { ++times_called; };

    level->set_show_bounding_boxes(true);
    ASSERT_EQ(times_called, 1u);
}

TEST(Level, ItemsNotRenderedWhenDisabled)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));
    ON_CALL(*room, alternate_mode).WillByDefault(Return(IRoom::AlternateMode::IsAlternate));

    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    NiceMock<MockShader> shader;
    auto shader_storage = mock_shared<MockShaderStorage>();
    EXPECT_CALL(*shader_storage, get).WillRepeatedly(Return(&shader));

    EXPECT_CALL(*room, render(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::Entities, false), A<const std::unordered_set<uint32_t>&>())).Times(1);
    EXPECT_CALL(*room, render_contained(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::Entities, false))).Times(1);

    auto level = register_test_module()
        .with_device(device)
        .with_shader_storage(shader_storage)
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    level->set_alternate_mode(true);
    level->set_show_items(false);

    NiceMock<MockCamera> camera;
    level->render(camera, false);
}

TEST(Level, ItemsRenderedWhenEnabled)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));
    ON_CALL(*room, alternate_mode).WillByDefault(Return(IRoom::AlternateMode::IsAlternate));

    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    NiceMock<MockShader> shader;
    auto shader_storage = mock_shared<MockShaderStorage>();
    EXPECT_CALL(*shader_storage, get).WillRepeatedly(Return(&shader));

    EXPECT_CALL(*room, render(A<const ICamera&>(), A<IRoom::SelectionMode>(), RenderFilter::Default, A<const std::unordered_set<uint32_t>&>())).Times(1);
    EXPECT_CALL(*room, render_contained(A<const ICamera&>(), A<IRoom::SelectionMode>(), RenderFilter::Default)).Times(1);

    auto level = register_test_module()
        .with_device(device)
        .with_shader_storage(shader_storage)
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    level->set_alternate_mode(true);

    NiceMock<MockCamera> camera;
    level->render(camera, false);
}

TEST(Level, SetRoomVisibilty)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, set_visible(false)).Times(1);

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    bool level_changed_raised = false;
    auto token = level->on_level_changed += [&]() { level_changed_raised = true; };

    level->set_room_visibility(0, false);

    ASSERT_TRUE(level_changed_raised);
}

TEST(Level, RoomNotRenderedWhenNotVisible)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(false));
    ON_CALL(*room, alternate_mode).WillByDefault(Return(IRoom::AlternateMode::IsAlternate));

    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    NiceMock<MockShader> shader;
    auto shader_storage = mock_shared<MockShaderStorage>();
    EXPECT_CALL(*shader_storage, get).WillRepeatedly(Return(&shader));

    EXPECT_CALL(*room, render(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::Rooms, false), A<const std::unordered_set<uint32_t>&>())).Times(0);
    EXPECT_CALL(*room, render_contained(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::Rooms, false))).Times(0);

    auto level = register_test_module()
        .with_device(device)
        .with_shader_storage(shader_storage)
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    level->set_alternate_mode(true);

    NiceMock<MockCamera> camera;
    level->render(camera, false);
}

TEST(Level, SelectedItem)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_entities()).WillByDefault(Return(5));
    ON_CALL(mock_level, num_rooms()).WillByDefault(Return(1));

    std::vector<std::shared_ptr<IItem>> items;

    uint32_t entity_source_called = 0;
    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_entity_source(
            [&](auto&&...)
            {
                auto entity = mock_shared<MockItem>();
                ON_CALL(*entity, number).WillByDefault(Return(entity_source_called));
                ++entity_source_called;
                items.push_back(entity);
                return entity;
            })
        .build();

    std::shared_ptr<IItem> raised;
    auto token = level->on_item_selected += [&](auto t) { raised = t.lock(); };

    ASSERT_EQ(level->selected_item().lock(), nullptr);
    level->set_selected_item(items[4]);
    ASSERT_EQ(level->selected_item().lock(), items[4]);
    ASSERT_EQ(raised, items[4]);
}

TEST(Level, SelectedLight)
{
    tr3_room room{ };
    room.lights.resize(5);
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_rooms).WillByDefault(Return(1));
    ON_CALL(mock_level, get_room).WillByDefault(Return(room));
    
    uint32_t light_source_called = 0;
    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_light_source(
            [&](auto&&...)
            {
                auto light = mock_shared<MockLight>();
                ON_CALL(*light, number).WillByDefault(Return(light_source_called));
                ++light_source_called;
                return light;
            })
        .build();
    ASSERT_EQ(level->selected_light(), std::nullopt);
    level->set_selected_light(4);
    ASSERT_EQ(level->selected_light(), 4);
}

TEST(Level, SelectedTrigger)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_rooms()).WillByDefault(Return(1));

    uint32_t trigger_source_called = 0;
    std::vector<std::shared_ptr<ITrigger>> triggers;

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_room_source(
            [&](auto&&...)
            {
                auto room = mock_shared<MockRoom>();
                std::vector<std::shared_ptr<ISector>> sectors;
                auto sector = mock_shared<MockSector>();
                ON_CALL(*sector, flags).WillByDefault(Return(SectorFlag::Trigger));
                sectors.resize(5, sector);
                ON_CALL(*room, sectors).WillByDefault(Return(sectors));
                return room;
            })
        .with_trigger_source(
            [&](auto&&...)
            {
                auto trigger = mock_shared<MockTrigger>();
                ON_CALL(*trigger, number).WillByDefault(Return(trigger_source_called));
                ++trigger_source_called;
                triggers.push_back(trigger);
                return trigger;
            })
        .build();

    std::shared_ptr<ITrigger> raised;
    auto token = level->on_trigger_selected += [&](auto t) { raised = t.lock(); };

    ASSERT_EQ(level->selected_trigger(), std::nullopt);
    level->set_selected_trigger(4);
    ASSERT_EQ(level->selected_trigger(), 4);
    ASSERT_EQ(raised, triggers[4]);
}

TEST(Level, Trigger)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_rooms()).WillByDefault(Return(1));

    uint32_t trigger_source_called = 0;
    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_room_source(
            [&](auto&&...)
            {
                auto room = mock_shared<MockRoom>();
                std::vector<std::shared_ptr<ISector>> sectors;
                auto sector = mock_shared<MockSector>();
                ON_CALL(*sector, flags).WillByDefault(Return(SectorFlag::Trigger));
                sectors.resize(5, sector);
                ON_CALL(*room, sectors).WillByDefault(Return(sectors));
                return room;
            })
        .with_trigger_source(
            [&](auto&&...)
            {
                auto trigger = mock_shared<MockTrigger>();
                ON_CALL(*trigger, number).WillByDefault(Return(trigger_source_called));
                ++trigger_source_called;
                return trigger;
            })
        .build();

    auto trigger = level->trigger(0).lock();
    ASSERT_NE(trigger, nullptr);
    ASSERT_EQ(trigger->number(), 0);
}

TEST(Level, TriggerNotFound)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_rooms()).WillByDefault(Return(1));

    uint32_t trigger_source_called = 0;
    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_room_source(
            [&](auto&&...)
            {
                auto room = mock_shared<MockRoom>();
                std::vector<std::shared_ptr<ISector>> sectors;
                auto sector = mock_shared<MockSector>();
                ON_CALL(*sector, flags).WillByDefault(Return(SectorFlag::Trigger));
                sectors.resize(5, sector);
                ON_CALL(*room, sectors).WillByDefault(Return(sectors));
                return room;
            })
        .with_trigger_source(
            [&](auto&&...)
            {
                auto trigger = mock_shared<MockTrigger>();
                ON_CALL(*trigger, number).WillByDefault(Return(trigger_source_called));
                ++trigger_source_called;
                return trigger;
            })
        .build();

    auto trigger = level->trigger(10).lock();
    ASSERT_EQ(trigger, nullptr);
}

TEST(Level, Item)
{
    tr2_entity entity{};
    entity.Room = 0;
    entity.TypeID = 123;

    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_entities()).WillByDefault(Return(1));
    ON_CALL(mock_level, get_entity(0)).WillByDefault(Return(entity));
    ON_CALL(mock_level, num_rooms()).WillByDefault(Return(1));

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .build();

    auto item = level->item(0).lock();
    ASSERT_TRUE(item);
}

TEST(Level, ItemNotFound)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .build();

    auto item = level->item(0).lock();
    ASSERT_FALSE(item);
}

TEST(Level, Light)
{
    tr3_room room{ };
    room.lights.resize(1);

    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_rooms()).WillByDefault(Return(1));
    ON_CALL(mock_level, get_room).WillByDefault(Return(room));

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .build();

    auto light = level->light(0).lock();
    ASSERT_NE(light, nullptr);
    ASSERT_EQ(light->number(), 0);
}

TEST(Level, LightNotFound)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_rooms()).WillByDefault(Return(1));

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .build();

    auto light = level->light(0).lock();
    ASSERT_EQ(light, nullptr);
}

TEST(Level, MeshSetBuilt)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_models()).WillByDefault(Return(1));
    ON_CALL(mock_level, get_model(0)).WillByDefault(Return(trlevel::tr_model{ 100 }));

    auto level = register_test_module().with_level(std::move(mock_level_ptr)).build();

    ASSERT_TRUE(level->has_model(100));
    ASSERT_FALSE(level->has_model(123));
}

TEST(Level, SetShowRoomsRaisesLevelChangedEvent)
{
    auto level = register_test_module().build();

    uint32_t times_called = 0;
    auto token = level->on_level_changed += [&](auto&&...) { ++times_called; };

    level->set_show_rooms(true);
    ASSERT_EQ(times_called, 1u);
}

TEST(Level, CameraSinksNotRenderedWhenDisabled)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));
    ON_CALL(*room, alternate_mode).WillByDefault(Return(IRoom::AlternateMode::IsAlternate));

    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    NiceMock<MockShader> shader;
    auto shader_storage = mock_shared<MockShaderStorage>();
    EXPECT_CALL(*shader_storage, get).WillRepeatedly(Return(&shader));

    EXPECT_CALL(*room, render(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::CameraSinks, false), A<const std::unordered_set<uint32_t>&>())).Times(1);
    EXPECT_CALL(*room, render_contained(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::CameraSinks, false))).Times(1);

    auto level = register_test_module()
        .with_device(device)
        .with_shader_storage(shader_storage)
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    level->set_alternate_mode(true);
    level->set_show_camera_sinks(false);

    NiceMock<MockCamera> camera;
    level->render(camera, false);
}

TEST(Level, CameraSinksRenderedWhenEnabled)
{
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, visible).WillByDefault(Return(true));
    ON_CALL(*room, alternate_mode).WillByDefault(Return(IRoom::AlternateMode::IsAlternate));

    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    NiceMock<MockShader> shader;
    auto shader_storage = mock_shared<MockShaderStorage>();
    EXPECT_CALL(*shader_storage, get).WillRepeatedly(Return(&shader));

    EXPECT_CALL(*room, render(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::CameraSinks, true), A<const std::unordered_set<uint32_t>&>())).Times(1);
    EXPECT_CALL(*room, render_contained(A<const ICamera&>(), A<IRoom::SelectionMode>(), set_flag(RenderFilter::Default, RenderFilter::CameraSinks, true))).Times(1);

    auto level = register_test_module()
        .with_device(device)
        .with_shader_storage(shader_storage)
        .with_level(std::move(mock_level_ptr))
        .with_room_source([&](auto&&...) { return room; })
        .build();

    NiceMock<MockCamera> camera;

    level->set_alternate_mode(true);
    level->set_show_camera_sinks(true);
    level->render(camera, false);
}

TEST(Level, SetShowLightingRaisesLevelChangedEvent)
{
    auto level = register_test_module().build();

    uint32_t times_called = 0;
    auto token = level->on_level_changed += [&](auto&&...) { ++times_called; };

    level->set_show_lighting(true);
    ASSERT_EQ(times_called, 1u);
}

TEST(Level, SkidooGenerated)
{
    const tr2_entity driver { .TypeID = 52 };
    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    ON_CALL(mock_level, num_entities).WillByDefault(Return(1));
    EXPECT_CALL(mock_level, get_entity(0)).WillRepeatedly(Return(driver));
    ON_CALL(mock_level, get_version).WillByDefault(Return(trlevel::LevelVersion::Tomb2));

    std::vector<tr2_entity> entities;

    auto level = register_test_module()
        .with_level(std::move(mock_level_ptr))
        .with_entity_source(
            [&](auto&& level, const trlevel::tr2_entity& type, auto&&...)
            {
                entities.push_back(type);
                return mock_shared<MockItem>();
            })
        .build();

    ASSERT_EQ(entities.size(), 2);
    ASSERT_EQ(entities[0].TypeID, 52);
    ASSERT_EQ(entities[1].TypeID, 51);
}

TEST(Level, StaticMeshChangingRaisesLevelChangedEvent)
{
    auto static_mesh = mock_shared<MockStaticMesh>();

    auto room = mock_shared<MockRoom>();
    ON_CALL(*room, static_meshes).WillByDefault(Return(std::vector<std::weak_ptr<IStaticMesh>>{ static_mesh }));

    auto [mock_level_ptr, mock_level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));

    auto level = register_test_module().with_level(std::move(mock_level_ptr))
        .with_room_source(
            [&](auto&&...)
            {
                return room;
            }).build();

    uint32_t times_called = 0;
    auto token = level->on_level_changed += [&](auto&&...) { ++times_called; };

    static_mesh->on_changed();
    ASSERT_EQ(times_called, 1u);
}

TEST(Level, SetShowSoundSourcesRaisesLevelChangedEvent)
{
    auto level = register_test_module().build();

    bool raised = false;
    auto token = level->on_level_changed += capture_called(raised);

    level->set_show_sound_sources(true);
    ASSERT_EQ(raised, true);
}
