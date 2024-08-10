#include "pch.h"
#include "CameraControlsTests.h"

#include <trview.app/UI/CameraControls.h>

using namespace trview;

void register_camera_controls_tests(ImGuiTestEngine* engine)
{
    test<CameraControls>(engine, "Camera Controls", "Axis Raises Mode Selected",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            std::optional<ICamera::Mode> selected_mode;
            auto token = controls.on_mode_selected += [&](auto mode)
            {
                selected_mode = mode;
            };

            ctx->SetRef("Camera Controls");
            ctx->ComboClick("##Mode/Axis");

            IM_CHECK_EQ(selected_mode.has_value(), true);
            IM_CHECK_EQ(selected_mode.value(), ICamera::Mode::Axis);
        });

    test<CameraControls>(engine, "Camera Controls", "Free Raises Mode Selected",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            std::optional<ICamera::Mode> selected_mode;
            auto token = controls.on_mode_selected += [&](auto mode)
            {
                selected_mode = mode;
            };

            ctx->SetRef("Camera Controls");
            ctx->ComboClick("##Mode/Free");

            IM_CHECK_EQ(selected_mode.has_value(), true);
            IM_CHECK_EQ(selected_mode.value(), ICamera::Mode::Free);
        });

    test<CameraControls>(engine, "Camera Controls", "Orbit Raises Mode Selected",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            std::optional<ICamera::Mode> selected_mode;
            auto token = controls.on_mode_selected += [&](auto mode)
            {
                selected_mode = mode;
            };

            ctx->SetRef("Camera Controls");
            ctx->ComboClick("##Mode/Orbit");

            IM_CHECK_EQ(selected_mode.has_value(), true);
            IM_CHECK_EQ(selected_mode.value(), ICamera::Mode::Orbit);
        });

    test<CameraControls>(engine, "Camera Controls", "Ortho Raises Projection Mode",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            std::optional<ProjectionMode> selected_mode;
            auto token = controls.on_projection_mode_selected += [&](auto mode)
            {
                selected_mode = mode;
            };

            ctx->SetRef("Camera Controls");
            ctx->ComboClick("##Projection/Orthographic");

            IM_CHECK_EQ(selected_mode.has_value(), true);
            IM_CHECK_EQ(selected_mode.value(), ProjectionMode::Orthographic);
        });

    test<CameraControls>(engine, "Camera Controls", "Perspective Raises Projection Mode",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            std::optional<ProjectionMode> selected_mode;
            auto token = controls.on_projection_mode_selected += [&](auto mode)
            {
                selected_mode = mode;
            };

            ctx->SetRef("Camera Controls");
            ctx->ComboClick("##Projection/Perspective");

            IM_CHECK_EQ(selected_mode.has_value(), true);
            IM_CHECK_EQ(selected_mode.value(), ProjectionMode::Perspective);
        });

    test<CameraControls>(engine, "Camera Controls", "Reset Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            bool raised = false;
            auto token = controls.on_reset += [&]()
            {
                raised = true;
            };

            ctx->SetRef("Camera Controls");
            ctx->ItemClick("Reset");

            IM_CHECK_EQ(raised, true);
        });

    test<CameraControls>(engine, "Camera Controls", "Set Mode Updates Combo",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            ctx->SetRef("Camera Controls");

            auto id = ctx->ItemInfo("##Mode")->ID;
            IM_CHECK_EQ(ItemText(ctx, id), "Orbit");
            controls.set_mode(ICamera::Mode::Axis);
            ctx->Yield();
            IM_CHECK_EQ(ItemText(ctx, id), "Axis");
        });

    test<CameraControls>(engine, "Camera Controls", "Set Projection Mode Updates Combo",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraControls>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<CameraControls>();
            ctx->SetRef("Camera Controls");
            
            auto id = ctx->ItemInfo("##Projection")->ID;
            IM_CHECK_EQ(ItemText(ctx, id), "Perspective");
            controls.set_projection_mode(ProjectionMode::Orthographic);
            ctx->Yield();
            IM_CHECK_EQ(ItemText(ctx, id), "Orthographic");
        });
}