#include "pch.h"
#include "SettingsWindowTests.h"

#include <format>

#include <trview.app/UI/SettingsWindow.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.common/Mocks/Windows/IDialogs.h>

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
            std::shared_ptr<IDialogs> dialogs{ mock_shared<MockDialogs>() };
            std::shared_ptr<IShell> shell{ mock_shared<MockShell>() };

            std::unique_ptr<SettingsWindow> build()
            {
                return std::make_unique<SettingsWindow>(dialogs, shell);
            }

            test_module& with_dialogs(const std::shared_ptr<IDialogs>& dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_shell(const std::shared_ptr<IShell>& shell)
            {
                this->shell = shell;
                return *this;
            }
        };

        return test_module{};
    }

    template <typename T>
    struct MockWrapper
    {
        std::unique_ptr<T> ptr;
    };

    template <typename T>
    void render(const MockWrapper<T>& wrapper)
    {
        if (wrapper.ptr)
        {
            wrapper.ptr->render();
        }
    }
}

void register_settings_window_tests(ImGuiTestEngine* engine)
{
    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Add Plugin Directories",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();

            auto dialogs = mock_shared<MockDialogs>();
            ON_CALL(*dialogs, open_folder).WillByDefault(Return("test"));
            controls.ptr = register_test_module().with_dialogs(dialogs).build();;

            std::optional<std::vector<std::string>> raised;
            auto token = controls.ptr->on_plugin_directories += [&](const auto& value)
            {
                raised = value;
            };

            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Plugins");
            ctx->ItemClick("TabBar/Plugins/Directories/Add");

            const std::vector<std::string> expected{ "test" };
            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), expected);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Changing Max Recent Files Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();

            std::optional<uint32_t> received_value;
            auto token = controls.ptr->on_max_recent_files += [&](uint32_t value)
            {
                received_value = value;
            };

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            ctx->ItemClick("TabBar/General/Recent Files/+");

            ASSERT_TRUE(received_value.has_value());
            ASSERT_EQ(received_value.value(), 11);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Acceleration Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();

            std::optional<bool> received_value;
            auto token = controls.ptr->on_camera_acceleration += [&](bool value)
            {
                received_value = value;
            };

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Acceleration"), false);
            ctx->ItemCheck("TabBar/Camera/Acceleration");

            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value.value(), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Acceleration Rate Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();

            std::optional<float> received_value;
            auto token = controls.ptr->on_camera_acceleration_rate += [&](float value)
            {
                received_value = value;
            };

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            ctx->ItemClick("TabBar/Camera/Acceleration Rate");
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value.value(), 0.5f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Camera Degrees Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();

            std::optional<bool> received_value;
            auto token = controls.ptr->on_camera_display_degrees += [&](bool value)
            {
                received_value = value;
            };

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), false);
            ctx->ItemCheck("TabBar/Camera/Use degrees for camera angle display");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value.value(), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Camera Degrees Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), false);
            controls.ptr->set_camera_display_degrees(true);
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Open Camera/Sink Window at startup Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();

            std::optional<bool> received_value;
            auto token = controls.ptr->on_camera_sink_startup += [&](bool value)
            {
                received_value = value;
            };

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Camera\\/Sink Window at startup"), false);
            ctx->ItemCheck("TabBar/General/Open Camera\\/Sink Window at startup");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Camera\\/Sink Window at startup"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value.value(), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Fov Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();
            controls.ptr->set_fov(45);

            std::optional<float> received_value;
            auto token = controls.ptr->on_camera_fov += [&](float value)
            {
                received_value = value;
            };

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            ctx->ItemClick("TabBar/Camera/Camera FOV");
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value.value(), 77.5f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Go To Lara Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();;
            controls.ptr->toggle_visibility();

            std::optional<bool> received_value;
            auto token = controls.ptr->on_go_to_lara += [&](bool value)
            {
                received_value = value;
            };

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Select Lara when level is opened"), false);
            ctx->ItemCheck("TabBar/General/Select Lara when level is opened");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Select Lara when level is opened"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value.value(), true);
        });
}
