#include "pch.h"
#include "SettingsWindowTests.h"

#include <format>

#include <trview.app/UI/SettingsWindow.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.app/Mocks/UI/IFonts.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>
#include <trview.tests.common/Event.h>

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
            std::shared_ptr<IFonts> fonts{ mock_shared<MockFonts>() };
            std::shared_ptr<ITextureStorage> texture_storage{ mock_shared<MockTextureStorage>() };

            std::unique_ptr<SettingsWindow> build()
            {
                return std::make_unique<SettingsWindow>(dialogs, shell, fonts, texture_storage);
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

            test_module& with_fonts(const std::shared_ptr<IFonts>& fonts)
            {
                this->fonts = fonts;
                return *this;
            }
        };

        return test_module{};
    }
}

void register_settings_window_tests(ImGuiTestEngine* engine)
{
    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Changing Font Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            const std::vector<FontSetting> available_fonts
            {
                {.name = "name", .filename = "filename", .size = 12 },
                {.name = "name2", .filename = "filename2", .size = 13 }
            };

            const std::unordered_map<std::string, FontSetting> registered_fonts
            {
                { "Default", { .name = "name", .filename = "filename", .size = 12 } },
                { "Console", { .name = "name2", .filename = "filename2", .size = 13 } }
            };

            auto fonts = mock_shared<MockFonts>();
            ON_CALL(*fonts, list_fonts).WillByDefault(Return(available_fonts));
            ON_CALL(*fonts, fonts).WillByDefault(Return(registered_fonts));

            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().with_fonts(fonts).build();
            controls.ptr->toggle_visibility();

            std::optional<std::pair<std::string, FontSetting>> received_value;
            auto token = controls.ptr->on_font += [&](auto&& key, auto&& value)
            {
                received_value = { key, value };
            };

            ctx->ItemClick("Settings/TabBar/Fonts");
            ctx->SetRef("Settings/TabBar/Fonts/Fonts List");
            ctx->ComboClick("##Default/name2");

            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->first, "Default");
            IM_CHECK_EQ(received_value->second.name, "name2");
            IM_CHECK_EQ(received_value->second.filename, "filename2");
            IM_CHECK_EQ(received_value->second.size, 12);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Changing Max Recent Files Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            ctx->ItemClick("TabBar/General/Recent Files/+");

            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->max_recent_files, 11);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Acceleration Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Acceleration"), true);
            ctx->ItemUncheck("TabBar/Camera/Acceleration");

            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->camera_acceleration, false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Acceleration Rate Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .camera_acceleration_rate = 0.1f });

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            ctx->ItemClick("TabBar/Camera/Acceleration Rate");
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->camera_acceleration_rate, 0.5f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Camera Degrees Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), true);
            ctx->ItemUncheck("TabBar/Camera/Use degrees for camera angle display");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), false);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->camera_display_degrees, false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Open Camera/Sink Window at startup Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Camera\\/Sink Window at startup"), false);
            ctx->ItemCheck("TabBar/General/Open Camera\\/Sink Window at startup");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Camera\\/Sink Window at startup"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->camera_sink_startup, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Fov Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            ctx->ItemClick("TabBar/Camera/Camera FOV");
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->fov, 77.5f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Go To Lara Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Select Lara when level is opened"), true);
            ctx->ItemUncheck("TabBar/General/Select Lara when level is opened");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Select Lara when level is opened"), false);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->go_to_lara, false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Invert Map Controls Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Minimap");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Minimap/Invert map controls"), false);
            ctx->ItemCheck("TabBar/Minimap/Invert map controls");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Minimap/Invert map controls"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->invert_map_controls, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Invert Vertical Pan Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Invert vertical panning"), true);
            ctx->ItemUncheck("TabBar/Camera/Invert vertical panning");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Invert vertical panning"), false);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->invert_vertical_pan, false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Items Window on Startup Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Items Window at startup"), false);
            ctx->ItemCheck("TabBar/General/Open Items Window at startup");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Items Window at startup"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->items_startup, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Movement Speed Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .camera_movement_speed = 0.1f });

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            ctx->ItemClick("TabBar/Camera/Movement Speed");
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->camera_movement_speed, 0.5f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Orbit Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Switch to orbit on selection"), true);
            ctx->ItemUncheck("TabBar/Camera/Switch to orbit on selection");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Switch to orbit on selection"), false);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->auto_orbit, false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Randomizer Tools Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Enable Randomizer Tools"), false);
            ctx->ItemCheck("TabBar/General/Enable Randomizer Tools");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Enable Randomizer Tools"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->randomizer_tools, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Reset FOV Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .fov = 10.0f });

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            ctx->ItemClick("TabBar/Camera/Reset##Fov");

            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->fov, 45.0f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Rooms Window on Startup Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Rooms Window at startup"), false);
            ctx->ItemCheck("TabBar/General/Open Rooms Window at startup");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Rooms Window at startup"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->rooms_startup, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Route Window on Startup Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Route Window at startup"), false);
            ctx->ItemCheck("TabBar/General/Open Route Window at startup");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Route Window at startup"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->route_startup, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Sensitivity Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            ctx->ItemClick("TabBar/Camera/Sensitivity");
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->camera_sensitivity, 0.5f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Statics Window on Startup Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Statics Window at startup"), false);
            ctx->ItemCheck("TabBar/General/Open Statics Window at startup");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Statics Window at startup"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->statics_startup, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Triggers Window on Startup Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Triggers Window at startup"), false);
            ctx->ItemCheck("TabBar/General/Open Triggers Window at startup");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Triggers Window at startup"), true);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->triggers_startup, true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Clicking Vsync Raises Event",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Visuals");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Visuals/Vsync"), true);
            ctx->ItemUncheck("TabBar/Visuals/Vsync");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Visuals/Vsync"), false);
            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->vsync, false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "On Minimap Colours Raised On Reset Normal",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            MapColours colours;
            colours.set_colour(SectorFlag::Death, Colour::Blue);
            controls.ptr->set_settings({ .map_colours = colours });

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Minimap");
            ctx->ItemClick("TabBar/Minimap/Reset##Death");

            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->map_colours.override_colours().size(), 0.0f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "On Minimap Colours Raised On Reset Special",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            MapColours colours;
            colours.set_colour(MapColours::Special::Default, Colour::Red);
            controls.ptr->set_settings({ .map_colours = colours });

            std::optional<UserSettings> received_value;
            auto token = controls.ptr->on_settings += trview::tests::capture(received_value);

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Minimap");
            ctx->ItemClick("TabBar/Minimap/Reset##Default");

            IM_CHECK_EQ(received_value.has_value(), true);
            IM_CHECK_EQ(received_value->map_colours.special_colours().size(), 0.0f);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Acceleration Rate Updates Slider",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .camera_acceleration_rate = 0.5f });

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Camera/Acceleration Rate")->ID), "0.500");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Acceleration Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Acceleration"), true);
            controls.ptr->set_settings({ .camera_acceleration = false });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Acceleration"), false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Camera Degrees Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), true);
            controls.ptr->set_settings({ .camera_display_degrees = false });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Use degrees for camera angle display"), false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Camera/Sink Window on Startup Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Camera\\/Sink Window at startup"), false);
            controls.ptr->set_settings({ .camera_sink_startup = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Camera\\/Sink Window at startup"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Default Route Colour Updates Colours",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .route_colour = Colour(0.5f, 0.75f, 1.0f) });

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Route");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Route/Default Route Colour/##X")->ID), "R:128");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Route/Default Route Colour/##Y")->ID), "G:191");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Route/Default Route Colour/##Z")->ID), "B:255");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Default Waypoint Colour Updates Colours",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .waypoint_colour = Colour(0.5f, 0.75f, 1.0f) });

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Route");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Route/Default Waypoint Colour/##X")->ID), "R:128");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Route/Default Waypoint Colour/##Y")->ID), "G:191");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Route/Default Waypoint Colour/##Z")->ID), "B:255");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set FOV Updates Slider",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .fov = 0.5f });

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Camera/Camera FOV")->ID), "0.500");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Go To Lara Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Select Lara when level is opened"), true);
            controls.ptr->set_settings({ .go_to_lara = false });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Select Lara when level is opened"), false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Invert Map Controls Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Minimap");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Minimap/Invert map controls"), false);
            controls.ptr->set_settings({ .invert_map_controls = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Minimap/Invert map controls"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Invert Vertical Pan Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Invert vertical panning"), true);
            controls.ptr->set_settings({ .invert_vertical_pan = false });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Invert vertical panning"), false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Items Window on Startup Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Items Window at startup"), false);
            controls.ptr->set_settings({ .items_startup = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Items Window at startup"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Map Colours Updates Colours",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Minimap");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##X")->ID), "R:  0");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##Y")->ID), "G:179");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##Z")->ID), "B:179");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##W")->ID), "A:255");

            MapColours colours;
            colours.set_colour(MapColours::Special::Default, Colour(0.25f, 0.5f, 0.75f, 1.0f));
            controls.ptr->set_settings({ .map_colours = colours });
            ctx->Yield();

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##X")->ID), "R:128");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##Y")->ID), "G:191");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##Z")->ID), "B:255");
            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Minimap/Default/##W")->ID), "A: 64");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Max Recent Files Updates Numeric Up Down",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .max_recent_files = 5 });

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/General/Recent Files")->ID), "5");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Movement Speed Updates Slider",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .camera_movement_speed = 0.5f });

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Camera/Movement Speed")->ID), "0.500");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Orbit Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Switch to orbit on selection"), true);
            controls.ptr->set_settings({ .auto_orbit = false });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Camera/Switch to orbit on selection"), false);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Randomizer Tools Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Enable Randomizer Tools"), false);
            controls.ptr->set_settings({ .randomizer_tools = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Enable Randomizer Tools"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Rooms Window on Startup Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Rooms Window at startup"), false);
            controls.ptr->set_settings({ .rooms_startup = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Rooms Window at startup"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Route Window on Startup Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Route Window at startup"), false);
            controls.ptr->set_settings({ .route_startup = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Route Window at startup"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Sensitivity Slider",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();
            controls.ptr->set_settings({ .camera_sensitivity = 0.5f });

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Camera");

            IM_CHECK_EQ(ItemText(ctx, ctx->ItemInfo("TabBar/Camera/Sensitivity")->ID), "0.500");
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Statics Window on Startup Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Statics Window at startup"), false);
            controls.ptr->set_settings({ .statics_startup = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Statics Window at startup"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set Triggers Window on Startup Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/General");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Triggers Window at startup"), false);
            controls.ptr->set_settings({ .triggers_startup = true });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/General/Open Triggers Window at startup"), true);
        });

    test<MockWrapper<SettingsWindow>>(engine, "Settings Window", "Set VSync Updates Checkbox",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<SettingsWindow>>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& controls = ctx->GetVars<MockWrapper<SettingsWindow>>();
            controls.ptr = register_test_module().build();
            controls.ptr->toggle_visibility();

            ctx->SetRef("Settings");
            ctx->ItemClick("TabBar/Visuals");
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Visuals/Vsync"), true);
            controls.ptr->set_settings({ .vsync = false });
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("TabBar/Visuals/Vsync"), false);
        });
}
