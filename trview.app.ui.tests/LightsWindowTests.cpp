#include "pch.h"
#include "LightsWindowTests.h"
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Windows/LightsWindow.h>
#include <trview.common/Mocks/Windows/IClipboard.h>

using namespace testing;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<LightsWindow> build()
            {
                return std::make_unique<LightsWindow>(clipboard);
            }
        };

        return test_module{};
    }

    struct LightsWindowContext final
    {
        std::shared_ptr<LightsWindow> ptr;
        std::vector<std::shared_ptr<ILight>> lights;

        void render()
        {
            if (ptr)
            {
                ptr->render();
            }
        }
    };
}

void register_lights_window_tests(ImGuiTestEngine* engine)
{
    test<LightsWindowContext>(engine, "Lights Window", "Fob Bulb Stats",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::FogBulb);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb4);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Position"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Intensity"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Density"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Radius"), true);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Light List Filtered When Room Set and Track Room Enabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto room_78 = mock_shared<MockRoom>()->with_number(78);
            auto light1 = mock_shared<MockLight>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(56));
            auto light2 = mock_shared<MockLight>()->with_number(1)->with_room(room_78);
            context.lights = { light1, light2 };
            context.ptr->set_lights({ light1, light2 });
            context.ptr->set_current_room(room_78);

            ctx->ItemClick("/**/Track##track");
            ctx->ItemCheck("/**/Room");
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Light List Not Filtered When Room Set and Track Room Disabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto room_78 = mock_shared<MockRoom>()->with_number(78);
            auto light1 = mock_shared<MockLight>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(56));
            auto light2 = mock_shared<MockLight>()->with_number(1)->with_room(room_78);
            context.lights = { light1, light2 };
            context.ptr->set_lights({ light1, light2 });
            context.ptr->set_current_room(room_78);

            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Light Selected Raised When Sync Item Enabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            std::weak_ptr<ILight> raised_light;
            auto token = context.ptr->on_light_selected += [&raised_light](const auto& light) { raised_light = light; };

            auto light1 = mock_shared<MockLight>()->with_number(0);
            auto light2 = mock_shared<MockLight>()->with_number(1);
            context.lights = { light1, light2 };
            context.ptr->set_lights({ light1, light2 });

            ctx->ItemClick("/**/1##1");

            ASSERT_EQ(raised_light.lock(), light2);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Light Selected Not Raised When Sync Item Disabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            std::weak_ptr<ILight> raised_light;
            auto token = context.ptr->on_light_selected += [&raised_light](const auto& light) { raised_light = light; };

            auto light1 = mock_shared<MockLight>()->with_number(0);
            auto light2 = mock_shared<MockLight>()->with_number(1);
            context.lights = { light1, light2 };
            context.ptr->set_lights({ light1, light2 });

            ctx->ItemUncheck("/**/Sync");
            ctx->ItemClick("/**/1##1");

            ASSERT_EQ(raised_light.lock(), nullptr);
        });

    test<LightsWindowContext>(engine, "Lights Window", "On Light Visibility Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0);
            auto light2 = mock_shared<MockLight>()->with_number(1)->with_updating_visible(false);
            EXPECT_CALL(*light2, set_visible(true)).Times(1);

            context.lights = { light1, light2 };
            context.ptr->set_lights({ light1, light2 });

            ctx->ItemUncheck("/**/##hide-1");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(light2.get()), true);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Point Stats TR1",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point)->with_level_version(trlevel::LevelVersion::Tomb1);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb1);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Colour"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Position"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Intensity"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Fade"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Hostpot"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Falloff"), false);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Point Stats TR2",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point)->with_level_version(trlevel::LevelVersion::Tomb2);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb2);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Colour"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Position"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Intensity"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Fade"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Hostpot"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Falloff"), false);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Point Stats TR3",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point)->with_level_version(trlevel::LevelVersion::Tomb3);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb3);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Colour"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Position"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Intensity"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Fade"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Hostpot"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Falloff"), false);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Point Stats TR4",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point)->with_level_version(trlevel::LevelVersion::Tomb4);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb4);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Colour"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Position"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Intensity"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Fade"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Hotspot"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Falloff"), true);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Shadow Stats",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Shadow)->with_level_version(trlevel::LevelVersion::Tomb4);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb4);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Colour"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Position"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Intensity"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Hotspot"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Falloff"), true);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Spot Stats",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Spot)->with_level_version(trlevel::LevelVersion::Tomb4);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb4);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Colour"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Position"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Direction"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Intensity"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Hotspot"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Falloff Angle"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Length"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Cutoff"), true);
        });

    test<LightsWindowContext>(engine, "Lights Window", "Sun Stats",
        [](ImGuiTestContext* ctx) { ctx->GetVars<LightsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LightsWindowContext>();
            context.ptr = register_test_module().build();

            auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Sun);
            context.lights = { light1 };
            context.ptr->set_lights({ light1 });
            context.ptr->set_selected_light(light1);
            context.ptr->set_level_version(trlevel::LevelVersion::Tomb4);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Type"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/#"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Room"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Colour"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/Stats/Direction"), true);
        });
}

