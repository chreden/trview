#include <trview.app/Elements/Light.h>
#include <trview.app/Mocks/Elements/IRoom.h>

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
            uint32_t number{ 0u };
            std::shared_ptr<IRoom> room{ mock_shared<MockRoom>() };
            tr_x_room_light light{};
            std::shared_ptr<trview::ILevel> level{ mock_shared<MockLevel>() };

            Light build()
            {
                return Light(mesh, number, room, light, level);
            }

            test_module& with_light(const tr_x_room_light& light)
            {
                this->light = light;
                return *this;
            }
        };

        return test_module{};
    }
}


TEST(Light, PointTR1)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb1;
    level_light.tr1.fade = 12500;
    level_light.tr1.intensity = 20000;
    level_light.tr1.x = 1024;
    level_light.tr1.y = 2048;
    level_light.tr1.z = 3072;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour::White);
    ASSERT_EQ(light.fade(), 12500);
    ASSERT_EQ(light.intensity(), 20000);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Point);
}

TEST(Light, PointTR2)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb2;
    level_light.tr2.fade1 = 12500;
    level_light.tr2.intensity1 = 20000;
    level_light.tr2.x = 1024;
    level_light.tr2.y = 2048;
    level_light.tr2.z = 3072;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour::White);
    ASSERT_EQ(light.fade(), 12500);
    ASSERT_EQ(light.intensity(), 20000);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Point);
}

TEST(Light, PointTR3)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb3;
    level_light.tr3.point.fade = 12500;
    level_light.tr3.point.intensity = 20000;
    level_light.tr3.x = 1024;
    level_light.tr3.y = 2048;
    level_light.tr3.z = 3072;
    level_light.tr3.colour = { 64, 128, 255 };
    level_light.tr3.type = LightTypeTR3::Point;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.250980407f, 0.501960814f, 1.0f));
    ASSERT_EQ(light.fade(), 12500);
    ASSERT_EQ(light.intensity(), 20000);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Point);
}

TEST(Light, SunTR3) 
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb3;
    level_light.tr3.sun.nx = 1024;
    level_light.tr3.sun.ny = 2048;
    level_light.tr3.sun.nz = 3072;
    level_light.tr3.colour = { 64, 128, 255 };
    level_light.tr3.type = LightTypeTR3::Sun;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.250980407f, 0.501960814f, 1.0f));
    ASSERT_EQ(light.direction(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Sun);
}

TEST(Light, PointTR4)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb4;
    level_light.tr4.x = 1024;
    level_light.tr4.y = 2048;
    level_light.tr4.z = 3072;
    level_light.tr4.colour = { 64, 128, 255 };
    level_light.tr4.light_type = LightType::Point;
    level_light.tr4.intensity = 12000;
    level_light.tr4.in = 1.0f;
    level_light.tr4.out = 0.5f;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.250980407f, 0.501960814f, 1.0f));
    ASSERT_EQ(light.intensity(), 12000);
    ASSERT_EQ(light.in(), 1.0f);
    ASSERT_EQ(light.out(), 0.5f);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Point);
}

TEST(Light, SunTR4)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb4;
    level_light.tr4.dx = 1024;
    level_light.tr4.dy = 2048;
    level_light.tr4.dz = 3072;
    level_light.tr4.colour = { 64, 128, 255 };
    level_light.tr4.light_type = LightType::Sun;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.250980407f, 0.501960814f, 1.0f));
    ASSERT_EQ(light.direction(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Sun);
}

TEST(Light, SpotTR4)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb4;
    level_light.tr4.x = 1024;
    level_light.tr4.y = 2048;
    level_light.tr4.z = 3072;
    level_light.tr4.colour = { 64, 128, 255 };
    level_light.tr4.light_type = LightType::Spot;
    level_light.tr4.in = 1.0f;
    level_light.tr4.out = 0.5f;
    level_light.tr4.cutoff = 0.75f;
    level_light.tr4.length = 20.0f;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.250980407f, 0.501960814f, 1.0f));
    ASSERT_EQ(light.in(), 1.0f);
    ASSERT_EQ(light.out(), 0.5f);
    ASSERT_EQ(light.cutoff(), 0.75f);
    ASSERT_EQ(light.length(), 20.0f);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Spot);
}

TEST(Light, FogBulbTR4)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb4;
    level_light.tr4.x = 1024;
    level_light.tr4.y = 2048;
    level_light.tr4.z = 3072;
    level_light.tr4.colour.Red = 128;
    level_light.tr4.out = 2.0f;
    level_light.tr4.light_type = LightType::FogBulb;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.density(), 0.501960814f);
    ASSERT_EQ(light.radius(), 2.0f);
    ASSERT_EQ(light.type(), LightType::FogBulb);
}

TEST(Light, ShadowTR4)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb4;
    level_light.tr4.x = 1024;
    level_light.tr4.y = 2048;
    level_light.tr4.z = 3072;
    level_light.tr4.colour = { 64, 128, 255 };
    level_light.tr4.light_type = LightType::Shadow;
    level_light.tr4.in = 1.0f;
    level_light.tr4.out = 0.5f;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.250980407f, 0.501960814f, 1.0f));
    ASSERT_EQ(light.in(), 1.0f);
    ASSERT_EQ(light.out(), 0.5f);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Shadow);
}

TEST(Light, PointTR5)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb5;
    level_light.tr5.position = { 1024, 2048, 3072 };
    level_light.tr5.colour = { 0.25f, 0.5f, 1.0f };
    level_light.tr5.light_type = LightType::Point;
    level_light.tr5.in = 1.0f;
    level_light.tr5.out = 0.5f;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.25f, 0.5f, 1.0f));
    ASSERT_EQ(light.in(), 1.0f);
    ASSERT_EQ(light.out(), 0.5f);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Point);
}

TEST(Light, SpotTR5)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb5;
    level_light.tr5.position = { 1024, 2048, 3072 };
    level_light.tr5.colour = { 0.25f, 0.5f, 1.0f };
    level_light.tr5.light_type = LightType::Spot;
    level_light.tr5.in = 1.0f;
    level_light.tr5.out = 0.5f;
    level_light.tr5.rad_in = 0.25f;
    level_light.tr5.rad_out = 0.75f;
    level_light.tr5.range = 20.0f;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.25f, 0.5f, 1.0f));
    ASSERT_EQ(light.in(), 1.0f);
    ASSERT_EQ(light.out(), 0.5f);
    ASSERT_EQ(light.rad_in(), 0.25f);
    ASSERT_EQ(light.rad_out(), 0.75f);
    ASSERT_EQ(light.range(), 20.0f);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Spot);
}

TEST(Light, SunTR5)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb5;
    level_light.tr5.direction = { 1024, 2048, 3072 };
    level_light.tr5.colour = { 0.25f, 0.5f, 1.0f };
    level_light.tr5.light_type = LightType::Sun;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.25f, 0.5f, 1.0f));
    ASSERT_EQ(light.direction(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Sun);
}

TEST(Light, ShadowTR5)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb5;
    level_light.tr5.position = { 1024, 2048, 3072 };
    level_light.tr5.colour = { 0.25f, 0.5f, 1.0f };
    level_light.tr5.light_type = LightType::Shadow;
    level_light.tr5.in = 1.0f;
    level_light.tr5.out = 0.5f;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.25f, 0.5f, 1.0f));
    ASSERT_EQ(light.in(), 1.0f);
    ASSERT_EQ(light.out(), 0.5f);
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.type(), LightType::Shadow);
}

TEST(Light, FogBulbTR5)
{
    tr_x_room_light level_light{};
    level_light.level_version = LevelVersion::Tomb5;
    level_light.tr5_fog.position = { 1024, 2048, 3072 };
    level_light.tr5_fog.colour = { 0.25f, 0.5f, 1.0f };
    level_light.tr5.light_type = LightType::FogBulb;
    level_light.tr5_fog.density = 1.5f;
    level_light.tr5_fog.radius = 2.0f;
    auto light = register_test_module().with_light(level_light).build();

    ASSERT_EQ(light.colour(), Colour(0.25f, 0.5f, 1.0f));
    ASSERT_EQ(light.position(), Vector3(1, 2, 3));
    ASSERT_EQ(light.density(), 1.5f);
    ASSERT_EQ(light.radius(), 2.0f);
    ASSERT_EQ(light.type(), LightType::FogBulb);
}

TEST(Light, VisibilityRaisesOnChanged)
{
    auto light = register_test_module().build();

    bool raised = false;
    auto token = light.on_changed += [&]() { raised = true; };
    light.set_visible(false);

    ASSERT_EQ(raised, true);
}
