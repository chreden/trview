#include "pch.h"
#include "RouteWindowTests.h"

#include <trview.common/Mocks/IFiles.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.tests.common/Mocks.h>

#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.app/Mocks/Routing/IRandomizerRoute.h>
#include <trview.app/Windows/RouteWindow.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>

using namespace testing;
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
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };
            std::shared_ptr<IDialogs> dialogs{ mock_shared<MockDialogs>() };
            std::shared_ptr<IFiles> files{ mock_shared<MockFiles>() };

            test_module& with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }

            test_module& with_dialogs(const std::shared_ptr<IDialogs>& dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_files(const std::shared_ptr<IFiles>& files)
            {
                this->files = files;
                return *this;
            }

            std::unique_ptr<RouteWindow> build()
            {
                return std::make_unique<RouteWindow>(clipboard, dialogs, files);
            }
        };
        return test_module{};
    }

    struct RouteWindowContext final
    {
        std::shared_ptr<RouteWindow> ptr;
        std::shared_ptr<IRoute> route;
        std::shared_ptr<IRoom> room;

        void render()
        {
            if (ptr)
            {
                ptr->render();
            }
        }
    };
}

void register_route_window_tests(ImGuiTestEngine* engine)
{
    test<RouteWindowContext>(engine, "Route Window", "Attach Save Button Does Not Load File When Cancelled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto dialogs = mock_shared<MockDialogs>();
            EXPECT_CALL(*dialogs, open_file).Times(1);

            auto files = mock_shared<MockFiles>();
            EXPECT_CALL(*files, load_file(A<const std::string&>())).Times(0);

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*route, set_unsaved).Times(0);

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_files(files).build();
            context.ptr->set_route(route);
            context.route = route;

            ctx->ItemClick("/**/Attach Save");
            ASSERT_FALSE(waypoint->has_save());

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(dialogs.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(files.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Attach Save Button Loads Save",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto dialogs = mock_shared<MockDialogs>();
            EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename" }));

            auto files = mock_shared<MockFiles>();
            EXPECT_CALL(*files, load_file(A<const std::string&>())).Times(1).WillRepeatedly(Return(std::vector<uint8_t>{ 0x1, 0x2 }));

            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, set_save_file(std::vector<uint8_t>{ 0x1, 0x2 })).Times(1);

            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*route, set_unsaved(true)).Times(1);

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_files(files).build();
            context.ptr->set_route(route);

            ctx->ItemClick("/**/Attach Save");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(dialogs.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(files.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Attach Save Button Shows Message on Error",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto dialogs = mock_shared<MockDialogs>();
            EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename" }));
            EXPECT_CALL(*dialogs, message_box).Times(1);

            auto files = mock_shared<MockFiles>();
            EXPECT_CALL(*files, load_file(A<const std::string&>())).Times(1).WillRepeatedly(Throw(std::exception()));

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*route, set_unsaved(true)).Times(0);

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_files(files).build();
            context.ptr->set_route(route);

            ctx->ItemClick("/**/Attach Save");

            ASSERT_FALSE(waypoint->has_save());

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(dialogs.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(files.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Adding Waypoint Notes Marks Route Unsaved",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            const Vector3 waypoint_pos{ 130, 250, 325 };
            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, position).WillRepeatedly(Return(waypoint_pos));
            EXPECT_CALL(*waypoint, set_notes).Times(AtLeast(1));
            EXPECT_CALL(*waypoint, set_notes(std::string("Test\n"))).Times(AtLeast(1));

            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*route, set_unsaved(true)).Times(AtLeast(1));

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);
            context.route = route;

            ctx->ItemInputValue("/**/Notes##notes", "Test");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Choose Randomizer Drop Down Updates Waypoint",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            RandomizerSettings settings;
            settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::String, std::string("One"), { std::string("One"), std::string("Two") } };
            context.ptr->set_randomizer_settings(settings);
            context.ptr->set_randomizer_enabled(true);

            IWaypoint::WaypointRandomizerSettings new_settings;

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRandomizerRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);
            context.route = route;

            ctx->Yield();
            ctx->SetRef(ctx->WindowInfo("Route###Route/Waypoint Details")->Window);
            ctx->ComboClick("Test 1/Two");

            IM_CHECK_NE(new_settings.find("test1"), new_settings.end());
            IM_CHECK_EQ(std::get<std::string>(new_settings["test1"]), "Two");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Clear Save Marks Route Unsaved",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
            EXPECT_CALL(*waypoint, set_save_file(std::vector<uint8_t>())).Times(1);

            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*route, set_unsaved(true)).Times(1);

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_route(route);

            ctx->ItemClick("/**/SAVEGAME.0", ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Remove");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Click Stat Shows Bubble",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);

            ctx->MouseMoveToVoid();
            IM_CHECK_EQ(ctx->ItemExists("/**/##Tooltip_00"), false);
            ctx->ItemClick("/**/Room Position");
            ctx->Yield(10);
            IM_CHECK_EQ(ctx->ItemExists("/**/##Tooltip_00"), true);

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Delete Waypoint Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);

            std::optional<uint32_t> raised;
            auto token = context.ptr->on_waypoint_deleted += [&](const auto& waypoint)
            {
                raised = waypoint;
            };

            ctx->ItemClick("/**/Delete Waypoint");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), 0);

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Export Save Button Does Not Save File When Cancelled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto dialogs = mock_shared<MockDialogs>();
            EXPECT_CALL(*dialogs, save_file).Times(1);

            auto files = mock_shared<MockFiles>();
            EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::vector<uint8_t>&>())).Times(0);

            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
            EXPECT_CALL(*waypoint, save_file).Times(0);
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_files(files).build();
            context.route = route;
            context.ptr->set_route(route);

            ctx->ItemClick("/**/SAVEGAME.0");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(files.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(dialogs.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Export Save Button Saves File",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto dialogs = mock_shared<MockDialogs>();
            EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename", 0 }));

            auto files = mock_shared<MockFiles>();
            EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::vector<uint8_t>&>())).Times(1);

            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
            EXPECT_CALL(*waypoint, save_file).Times(AtLeast(1)).WillRepeatedly(Return(std::vector<uint8_t>{ 0x1 }));

            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_files(files).build();
            context.route = route;
            context.ptr->set_route(route);

            ctx->ItemClick("/**/SAVEGAME.0");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(files.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(dialogs.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Export Save Button Shows Error on Failure",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto dialogs = mock_shared<MockDialogs>();
            EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename", 0 }));
            EXPECT_CALL(*dialogs, message_box).Times(1);

            auto files = mock_shared<MockFiles>();
            EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::vector<uint8_t>&>())).Times(1).WillRepeatedly(Throw(std::exception()));

            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
            EXPECT_CALL(*waypoint, save_file).Times(AtLeast(1)).WillRepeatedly(Return(std::vector<uint8_t>{ 0x1 }));
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_files(files).build();
            context.route = route;
            context.ptr->set_route(route);

            ctx->ItemClick("/**/SAVEGAME.0");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(files.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(dialogs.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "File Open Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            bool raised = false;
            auto token = context.ptr->on_route_open += [&]()
            {
                raised = true;
            };

            ctx->ItemClick("Route###Route/##menubar/File");
            ctx->ItemClick("##Menu_00/Open");

            IM_CHECK_EQ(raised, true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Import Route Button Does Not Raise Event When Cancelled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();
        });

    test<RouteWindowContext>(engine, "Route Window", "Level Switch Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            auto route = mock_shared<MockRandomizerRoute>();
            ON_CALL(*route, filenames).WillByDefault(Return(std::vector<std::string>{ "test1", "test2" }));
            context.ptr->set_route(route);

            std::optional<std::string> raised;
            auto token = context.ptr->on_level_switch += [&](const auto& value)
            {
                raised = value;
            };

            ctx->ItemClick("/**/test2");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), "test2");
        });

    test<RouteWindowContext>(engine, "Route Window", "New Randomizer Route Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            bool raised = false;
            auto token = context.ptr->on_new_randomizer_route += [&]()
            {
                raised = true;
            };

            context.ptr->set_randomizer_enabled(true);
            ctx->ItemClick("Route###Route/##menubar/File");
            ctx->ItemClick("##Menu_00/New");
            ctx->ItemClick("##Menu_01/Randomizer Route");

            IM_CHECK_EQ(raised, true);
        });

    test<RouteWindowContext>(engine, "Route Window", "New Route Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            bool raised = false;
            auto token = context.ptr->on_new_route += [&]()
            {
                raised = true;
            };

            ctx->ItemClick("Route###Route/##menubar/File");
            ctx->ItemClick("##Menu_00/New");

            IM_CHECK_EQ(raised, true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Randomizer Panel Creates UI From Settings",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRandomizerRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            context.route = route;
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);

            RandomizerSettings settings;
            settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Boolean };
            settings.settings["test2"] = { "Test 2", RandomizerSettings::Setting::Type::String, std::string("One"), { std::string("One"), std::string("Two"), std::string("Three") } };
            settings.settings["test3"] = { "Test 3", RandomizerSettings::Setting::Type::Number, 1.0f };
            context.ptr->set_randomizer_settings(settings);
            context.ptr->set_randomizer_enabled(true);

            ctx->Yield();
            ctx->SetRef(ctx->WindowInfo("Route###Route/Waypoint Details")->Window);
            IM_CHECK_EQ(ctx->ItemExists("Randomizer Flags/Test 1"), true);
            IM_CHECK_EQ(ctx->ItemExists("Test 2"), true);
            IM_CHECK_EQ(ctx->ItemExists("Test 3"), true);

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Randomizer Panel Visible Based on Route Type",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            RandomizerSettings settings;
            settings.settings["test"] = { "Test", RandomizerSettings::Setting::Type::Boolean };
            context.ptr->set_randomizer_settings(settings);

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Test"), false);

            auto rando_route = mock_shared<MockRandomizerRoute>();
            EXPECT_CALL(*rando_route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*rando_route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            context.ptr->set_route(rando_route);
            context.route = route;

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Test"), true);

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Reload Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            auto route = mock_shared<MockRoute>();
            ON_CALL(*route, filename).WillByDefault(Return("test"));
            context.ptr->set_route(route);

            bool raised = false;
            auto token = context.ptr->on_route_reload += [&]()
            {
                raised = true;
            };

            ctx->ItemClick("Route###Route/##menubar/File");
            ctx->ItemClick("##Menu_00/Reload");

            IM_CHECK_EQ(raised, true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Room Position Values Copied to Clipboard",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto clipboard = mock_shared<MockClipboard>();
            EXPECT_CALL(*clipboard, write(std::wstring(L"133120, 256000, 332800"))).Times(1);

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().with_clipboard(clipboard).build();

            const Vector3 waypoint_pos{ 130, 250, 325 };
            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, position).WillRepeatedly(Return(waypoint_pos));

            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));

            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);

            ctx->ItemClick("/**/Room Position");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(clipboard.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Save Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            bool raised = false;
            auto token = context.ptr->on_route_save += [&]()
            {
                raised = true;
            };

            ctx->ItemClick("Route###Route/##menubar/File");
            ctx->ItemClick("##Menu_00/Save");

            IM_CHECK_EQ(raised, true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Save As Raises Event",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            bool raised = false;
            auto token = context.ptr->on_route_save_as += [&]()
            {
                raised = true;
            };

            ctx->ItemClick("Route###Route/##menubar/File");
            ctx->ItemClick("##Menu_00/Save As");

            IM_CHECK_EQ(raised, true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Set Randomizer Number Updates Waypoint",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            RandomizerSettings settings;
            settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Number, 1.0f };
            context.ptr->set_randomizer_settings(settings);
            context.ptr->set_randomizer_enabled(true);

            IWaypoint::WaypointRandomizerSettings new_settings;

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRandomizerRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);

            ctx->ItemInputValue("/**/Test 1", "2");

            IM_CHECK_NE(new_settings.find("test1"), new_settings.end());
            IM_CHECK_EQ(std::get<float>(new_settings["test1"]), 2.0);

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Set Randomizer Text Updates Waypoint",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            RandomizerSettings settings;
            settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::String, std::string("One") };
            context.ptr->set_randomizer_settings(settings);
            context.ptr->set_randomizer_enabled(true);

            IWaypoint::WaypointRandomizerSettings new_settings;

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRandomizerRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);

            ctx->ItemInputValue("/**/Test 1", "Two");

            IM_CHECK_NE(new_settings.find("test1"), new_settings.end());
            IM_CHECK_EQ(std::get<std::string>(new_settings["test1"]), "Two");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Toggle Randomizer Bool Updates Waypoint",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            RandomizerSettings settings;
            settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Boolean, true };

            IWaypoint::WaypointRandomizerSettings new_settings;

            auto waypoint = mock_shared<MockWaypoint>();
            auto route = mock_shared<MockRandomizerRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));
            EXPECT_CALL(*waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));

            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();
            context.route = route;
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);
            context.ptr->set_randomizer_settings(settings);
            context.ptr->set_randomizer_enabled(true);

            ctx->ItemClick("/**/Test 1");

            ASSERT_NE(new_settings.find("test1"), new_settings.end());
            ASSERT_FALSE(std::get<bool>(new_settings["test1"]));
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });

    test<RouteWindowContext>(engine, "Route Window", "Waypoint Room Position Calculated Correctly",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RouteWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RouteWindowContext>();
            context.ptr = register_test_module().build();

            const Vector3 room_pos{ 102400, 204800, 307200 };
            const Vector3 waypoint_pos{ 130, 250, 325 };

            RoomInfo info{};
            info.x = static_cast<int32_t>(room_pos.x);
            info.yBottom = static_cast<int32_t>(room_pos.y);
            info.z = static_cast<int32_t>(room_pos.z);

            auto room = mock_shared<MockRoom>();
            EXPECT_CALL(*room, info).WillRepeatedly(Return(info));

            auto waypoint = mock_shared<MockWaypoint>();
            EXPECT_CALL(*waypoint, position).WillRepeatedly(Return(waypoint_pos));
            auto route = mock_shared<MockRoute>();
            EXPECT_CALL(*route, waypoints).WillRepeatedly(Return(1));
            EXPECT_CALL(*route, waypoint(An<uint32_t>())).WillRepeatedly(Return(waypoint));

            context.ptr->set_rooms({ room });
            context.ptr->set_route(route);
            context.ptr->select_waypoint(0);
            context.route = route;
            context.room = room;
            ctx->Yield();

            IM_CHECK_STR_EQ(RenderedText(ctx, ctx->ItemInfo("/**/Room Position")->ParentID).c_str(), "30720, 51200, 25600");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(room.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(route.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(waypoint.get()), true);
        });
}
