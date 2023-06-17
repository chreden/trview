#include <trview.app/Elements/CameraSink/CameraSink.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ILevel.h>

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
            std::shared_ptr<IMesh> mesh{ mock_shared<MockMesh>() };
            std::shared_ptr<ITextureStorage> texture_storage{ mock_shared<MockTextureStorage>() };
            uint32_t number{ 0u };
            ICameraSink::Type type{ ICameraSink::Type::Camera };
            tr_camera camera{};
            std::vector<std::weak_ptr<IRoom>> inferred_rooms;
            std::vector<std::weak_ptr<ITrigger>> triggers;
            std::shared_ptr<trview::ILevel> level{ mock_shared<MockLevel>() };

            CameraSink build()
            {
                return CameraSink(mesh, texture_storage, number, camera, type, inferred_rooms, triggers, level);
            }

            test_module& with_camera_sink(const tr_camera& camera)
            {
                this->camera = camera;
                return *this;
            }

            test_module& with_type(ICameraSink::Type type)
            {
                this->type = type;
                return *this;
            }

            test_module& with_level(std::shared_ptr<trview::ILevel> level)
            {
                this->level = level;
                return *this;
            }
        };

        return test_module{};
    }
}

TEST(CameraSink, Camera)
{
    auto room = mock_shared<MockRoom>();
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, room(20)).WillRepeatedly(Return(room));

    tr_camera level_camera{};
    level_camera.x = 1024;
    level_camera.y = 2048;
    level_camera.z = 3072;
    level_camera.Room = 20;
    level_camera.Flag = 101;
    auto camera_sink = register_test_module()
        .with_camera_sink(level_camera)
        .with_type(ICameraSink::Type::Camera)
        .with_level(level)
        .build();

    ASSERT_EQ(camera_sink.type(), ICameraSink::Type::Camera);
    ASSERT_EQ(camera_sink.position(), Vector3(1, 2, 3));
    ASSERT_TRUE(camera_sink.persistent());
    ASSERT_EQ(camera_sink.flag(), 101);
    ASSERT_EQ(camera_sink.room().lock(), room);
}

TEST(CameraSink, Sink)
{
    tr_camera level_camera{};
    level_camera.x = 1024;
    level_camera.y = 2048;
    level_camera.z = 3072;
    level_camera.Room = 20;
    level_camera.Flag = 101;
    auto camera_sink = register_test_module()
        .with_camera_sink(level_camera)
        .with_type(ICameraSink::Type::Sink)
        .build();

    ASSERT_EQ(camera_sink.type(), ICameraSink::Type::Sink);
    ASSERT_EQ(camera_sink.position(), Vector3(1, 2, 3));
    ASSERT_EQ(camera_sink.strength(), 20);
    ASSERT_EQ(camera_sink.box_index(), 101);
}