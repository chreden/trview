#include "pch.h"
#include "CameraPositionTests.h"

#include <trview.app/UI/CameraPosition.h>

using namespace trview;
using namespace DirectX::SimpleMath;

void register_camera_position_tests(ImGuiTestEngine* engine)
{
    test<CameraPosition>(engine, "Camera Position", "Close Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();
            camera_position.set_rotation(-1.5707963267f, 1);

            bool raised = false;
            auto token = camera_position.on_hidden += [&]()
                {
                    raised = true;
                };

            ctx->ItemClick("Camera Position/#CLOSE");

            IM_CHECK_EQ(raised, true);
        });

    test<CameraPosition>(engine, "Camera Position", "Coordinates Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();
            camera_position.set_position(Vector3(1, 2, 3));
            ctx->Yield();

            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("Camera Position/X")->ID).c_str(), "1024.000");
            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("Camera Position/Y")->ID).c_str(), "2048.000");
            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("Camera Position/Z")->ID).c_str(), "3072.000");
        });

    test<CameraPosition>(engine, "Camera Position", "Position Event Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();

            int times_called = 0;
            Vector3 new_position;

            auto token = camera_position.on_position_changed += [&times_called, &new_position](const auto& position)
            {
                ++times_called;
                new_position = position;
            };

            ctx->ItemInputValue("Camera Position/X", "1024");
            ctx->ItemInputValue("Camera Position/Y", "2048");
            ctx->ItemInputValue("Camera Position/Z", "3072");

            IM_CHECK_EQ(times_called, 3);
            // The position is scaled by diving by 1024.
            IM_CHECK_FLOAT_EQ_EPS(new_position.x, 1.0f);
            IM_CHECK_FLOAT_EQ_EPS(new_position.y, 2.0f);
            IM_CHECK_FLOAT_EQ_EPS(new_position.z, 3.0f);
        });

    test<CameraPosition>(engine, "Camera Position", "Rotation Correctly Converted",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();
            camera_position.set_rotation(1, 2);

            std::optional<std::tuple<float, float>> value;
            auto token = camera_position.on_rotation_changed += [&](auto yaw, auto pitch)
            {
                value = { yaw, pitch };
            };

            ctx->ItemInputValue("Camera Position/Yaw", "90");

            IM_CHECK_EQ(value.has_value(), true);
            IM_CHECK_FLOAT_EQ_EPS(std::get<0>(value.value()), 1.570796326f);
            IM_CHECK_FLOAT_EQ_EPS(std::get<1>(value.value()), 2.0f);
        });

    test<CameraPosition>(engine, "Camera Position", "Rotation Event Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();

            int times_called = 0;
            float new_yaw = 0;
            float new_pitch = 0;

            auto token = camera_position.on_rotation_changed += [&times_called, &new_yaw, &new_pitch](float yaw, float pitch)
            {
                ++times_called;
                new_yaw = yaw;
                new_pitch = pitch;
            };

            ctx->ItemInputValue("Camera Position/Yaw", "90");
            ctx->ItemInputValue("Camera Position/Pitch", "180");

            IM_CHECK_EQ(times_called, 2);
            IM_CHECK_FLOAT_EQ_EPS(new_yaw, 1.5707964f);
            IM_CHECK_FLOAT_EQ_EPS(new_pitch, 3.1415927f);
        });

    test<CameraPosition>(engine, "Camera Position", "Rotation Show Radians",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();
            camera_position.set_display_degrees(false);

            const auto pi = 3.1415926535897932384626433832795f;

            camera_position.set_rotation(pi, pi * 0.5f);
            ctx->Yield();

            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("Camera Position/Yaw")->ID).c_str(), "3.1416");
            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("Camera Position/Pitch")->ID).c_str(), "1.5708");
        });

    test<CameraPosition>(engine, "Camera Position", "Rotation Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();
            const auto pi = 3.1415926535897932384626433832795f;
            camera_position.set_rotation(pi, pi * 0.5f);

            ctx->Yield();

            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("Camera Position/Yaw")->ID).c_str(), "180.0000");
            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("Camera Position/Pitch")->ID).c_str(), "90.0000");
        });

    test<CameraPosition>(engine, "Camera Position", "Yaw Removes Negatives",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraPosition>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& camera_position = ctx->GetVars<CameraPosition>();
            camera_position.set_rotation(-1.5707963267f, 1);

            std::optional<std::tuple<float, float>> value;
            auto token = camera_position.on_rotation_changed += [&](auto yaw, auto pitch)
            {
                value = { yaw, pitch };
            };

            ctx->ItemInputValue("Camera Position/Pitch", "0");

            IM_CHECK_EQ(value.has_value(), true);
            IM_CHECK_FLOAT_EQ_EPS(std::get<0>(value.value()), 4.71238898f);
            IM_CHECK_FLOAT_EQ_EPS(std::get<1>(value.value()), 0);
        });
}