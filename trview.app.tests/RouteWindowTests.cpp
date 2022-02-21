#include <trview.app/Windows/RouteWindow.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include "TestImgui.h"


using namespace DirectX::SimpleMath;
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
            std::shared_ptr<IClipboard> clipboard{ std::make_shared<MockClipboard>() };
            std::shared_ptr<IDialogs> dialogs{ std::make_shared<MockDialogs>() };
            std::shared_ptr<IFiles> files{ std::make_shared<MockFiles>() };
            std::shared_ptr<IShell> shell{ std::make_shared<MockShell>() };
            trview::Window parent{ create_test_window(L"RouteWindowTests") };

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

            test_module& with_shell(const std::shared_ptr<IShell>& shell)
            {
                this->shell = shell;
                return *this;
            }

            std::unique_ptr<RouteWindow> build()
            {
                return std::make_unique<RouteWindow>(parent, clipboard, dialogs, files, shell);
            }
        };
        return test_module{};
    }
}

TEST(RouteWindow, WaypointRoomPositionCalculatedCorrectly)
{
    const Vector3 room_pos{ 102400, 204800, 307200 };
    const Vector3 waypoint_pos{ 130, 250, 325 };

    auto window = register_test_module().build();

    RoomInfo info {};
    info.x = room_pos.x;
    info.yBottom = room_pos.y;
    info.z = room_pos.z;

    auto room = std::make_shared<MockRoom>();
    EXPECT_CALL(*room, info).WillRepeatedly(Return(info));

    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, position).WillRepeatedly(Return(waypoint_pos));
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_rooms({ room });
    window->set_route(&route);

    window->select_waypoint(0);

    TestImgui imgui([&]() { window->render(); });
    auto rendered = imgui.rendered_text(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id(RouteWindow::Names::waypoint_stats));

    ASSERT_THAT(rendered, testing::Contains("30720, 51200, 25600"));
}

TEST(RouteWindow, PositionValuesCopiedToClipboard)
{
    auto clipboard = std::make_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write(std::wstring(L"133120, 256000, 332800"))).Times(1);

    auto window = register_test_module().with_clipboard(clipboard).build();

    const Vector3 waypoint_pos{ 130, 250, 325 };
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, position).WillRepeatedly(Return(waypoint_pos));

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_route(&route);
    window->select_waypoint(0);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .push(RouteWindow::Names::waypoint_stats)
        .id("Position"));
}

TEST(RouteWindow, RoomPositionValuesCopiedToClipboard)
{
    auto clipboard = std::make_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write(std::wstring(L"133120, 256000, 332800"))).Times(1);

    auto window = register_test_module().with_clipboard(clipboard).build();

    const Vector3 waypoint_pos{ 130, 250, 325 };
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, position).WillRepeatedly(Return(waypoint_pos));

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_route(&route);
    window->select_waypoint(0);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .push(RouteWindow::Names::waypoint_stats)
        .id("Room Position"));
}
/*
TEST(RouteWindow, AddingWaypointNotesMarksRouteUnsaved)
{
    const Vector3 waypoint_pos{ 130, 250, 325 };
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, position).WillRepeatedly(Return(waypoint_pos));
    EXPECT_CALL(waypoint, set_notes(std::wstring(L"Test"))).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);
    window->select_waypoint(0);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id(RouteWindow::Names::notes));
    imgui.enter_text("Test");
}*/

TEST(RouteWindow, ClearSaveMarksRouteUnsaved)
{
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(waypoint, set_save_file(std::vector<uint8_t>())).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id(RouteWindow::Names::clear_save));
}

TEST(RouteWindow, ExportRouteButtonRaisesEvent)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename", 0 }));

    std::optional<std::string> file_raised;
    auto window = register_test_module().with_dialogs(dialogs).build();
    auto token = window->on_route_export += [&](const auto& filename, const auto& is_rando)
    {
        file_raised = filename;
    };

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_list_panel)
        .id(RouteWindow::Names::export_button));

    ASSERT_TRUE(file_raised.has_value());
    ASSERT_EQ(file_raised, "filename");
}

TEST(RouteWindow, ExportRouteButtonDoesNotRaiseEventWhenCancelled)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1);

    bool file_raised = false;
    auto window = register_test_module().with_dialogs(dialogs).build();
    auto token = window->on_route_export += [&](auto&&...)
    {
        file_raised = true;
    };

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_list_panel)
        .id(RouteWindow::Names::export_button));

    ASSERT_FALSE(file_raised);
}

TEST(RouteWindow, ImportRouteButtonRaisesEvent)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename" }));

    std::optional<std::string> file_raised;
    auto window = register_test_module().with_dialogs(dialogs).build();
    auto token = window->on_route_import += [&](const auto& filename, bool is_rando)
    {
        file_raised = filename;
    };

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_list_panel)
        .id(RouteWindow::Names::import_button));

    ASSERT_TRUE(file_raised.has_value());
    ASSERT_EQ(file_raised, "filename");
}

TEST(RouteWindow, ImportRouteButtonDoesNotRaiseEventWhenCancelled)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1);

    bool file_raised = false;
    auto window = register_test_module().with_dialogs(dialogs).build();
    auto token = window->on_route_import += [&](const auto& filename, bool is_rando)
    {
        file_raised = true;
    };
    
    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_list_panel)
        .id(RouteWindow::Names::import_button));

    ASSERT_FALSE(file_raised);
}

TEST(RouteWindow, ExportSaveButtonSavesFile)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename", 0 }));

    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::vector<uint8_t>&>())).Times(1);

    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(waypoint, save_file).Times(AtLeast(1)).WillRepeatedly(Return(std::vector<uint8_t>{ 0x1 }));

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    auto window = register_test_module().with_dialogs(dialogs).with_files(files).build();
    window->set_route(&route);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id("SAVEGAME.0"));
}

TEST(RouteWindow, ExportSaveButtonShowsErrorOnFailure)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename", 0 }));
    EXPECT_CALL(*dialogs, message_box).Times(1);

    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::vector<uint8_t>&>())).Times(1).WillRepeatedly(Throw(std::exception()));

    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(waypoint, save_file).Times(AtLeast(1)).WillRepeatedly(Return(std::vector<uint8_t>{ 0x1 }));
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    auto window = register_test_module().with_dialogs(dialogs).with_files(files).build();
    window->set_route(&route);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id("SAVEGAME.0"));
}

TEST(RouteWindow, ExportSaveButtonDoesNotSaveFileWhenCancelled)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1);

    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::vector<uint8_t>&>())).Times(0);

    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, has_save).Times(AtLeast(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(waypoint, save_file).Times(0);
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    auto window = register_test_module().with_dialogs(dialogs).with_files(files).build();
    window->set_route(&route);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id("SAVEGAME.0"));
}

TEST(RouteWindow, AttachSaveButtonLoadsSave)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename" }));

    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, load_file).Times(1).WillRepeatedly(Return(std::vector<uint8_t>{ 0x1, 0x2 }));

    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, set_save_file(std::vector<uint8_t>{ 0x1, 0x2 })).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().with_dialogs(dialogs).with_files(files).build();
    window->set_route(&route);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id(RouteWindow::Names::attach_save));
}

TEST(RouteWindow, AttachSaveButtonShowsMessageOnError)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename" }));
    EXPECT_CALL(*dialogs, message_box).Times(1);

    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, load_file).Times(1).WillRepeatedly(Throw(std::exception()));

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(0);

    auto window = register_test_module().with_dialogs(dialogs).with_files(files).build();
    window->set_route(&route);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id(RouteWindow::Names::attach_save));

    ASSERT_FALSE(waypoint.has_save());
}

TEST(RouteWindow, AttachSaveButtonDoesNotLoadFileWhenCancelled)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1);

    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, load_file).Times(0);

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved).Times(0);

    auto window = register_test_module().with_dialogs(dialogs).with_files(files).build();
    window->set_route(&route);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .id(RouteWindow::Names::attach_save));
    ASSERT_FALSE(waypoint.has_save());
}

TEST(RouteWindow, ClickStatShowsBubble)
{
    auto window = register_test_module().build();

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    window->set_route(&route);
    window->select_waypoint(0);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Route")
        .push_child(RouteWindow::Names::waypoint_details_panel)
        .push(RouteWindow::Names::waypoint_stats)
        .id("Room Position"));
    ASSERT_NE(imgui.find_window("##Tooltip_00"), nullptr);
}
/*
TEST(RouteWindow, RandomizerPanelVisibleBasedOnSetting)
{
    auto window = register_test_module().build();
    auto randomizer_group = window->root_control()->find<ui::Control>(RouteWindow::Names::randomizer_group);
    ASSERT_NE(randomizer_group, nullptr);
    ASSERT_FALSE(randomizer_group->visible());
    window->set_randomizer_enabled(true);
    ASSERT_TRUE(randomizer_group->visible());
}

TEST(RouteWindow, WindowResizesWhenRandomizerEnabled)
{
    auto window = register_test_module().build();
    const auto size = window->window().size();
    window->set_randomizer_enabled(true);
    const auto new_size = window->window().size();
    ASSERT_GT(new_size.height, size.height);
}

TEST(RouteWindow, RandomizerPanelCreatesUIFromSettings)
{
    auto window = register_test_module().build();
    auto randomizer_group = window->root_control()->find<ui::Control>(RouteWindow::Names::randomizer_group);

    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Boolean };
    settings.settings["test2"] = { "Test 2", RandomizerSettings::Setting::Type::String, "One", { std::string("One"), std::string("Two"), std::string("Three") } };
    settings.settings["test3"] = { "Test 3", RandomizerSettings::Setting::Type::Number, 1.0f };
    window->set_randomizer_settings(settings);

    auto test1 = randomizer_group->find<ui::Checkbox>("test1");
    ASSERT_NE(test1, nullptr);

    auto test2 = randomizer_group->find<ui::Dropdown>("test2");
    ASSERT_NE(test2, nullptr);

    auto test3 = randomizer_group->find<ui::TextArea>("test3");
    ASSERT_NE(test3, nullptr);
}

TEST(RouteWindow, ToggleRandomizerBoolUpdatesWaypoint)
{
    auto window = register_test_module().build();
    auto randomizer_group = window->root_control()->find<ui::Control>(RouteWindow::Names::randomizer_group);

    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Boolean, true };
    window->set_randomizer_settings(settings);

    IWaypoint::WaypointRandomizerSettings new_settings;

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
    window->set_route(&route);
    window->select_waypoint(0);

    auto test1 = randomizer_group->find<ui::Checkbox>("test1");
    ASSERT_NE(test1, nullptr);
    ASSERT_TRUE(test1->state());

    test1->clicked(Point());
    ASSERT_NE(new_settings.find("test1"), new_settings.end());
    ASSERT_FALSE(std::get<bool>(new_settings["test1"]));
}

TEST(RouteWindow, ChooseRandomizerDropDownUpdatesWaypoint)
{
    auto window = register_test_module().build();
    auto randomizer_group = window->root_control()->find<ui::Control>(RouteWindow::Names::randomizer_group);

    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::String, std::string("One"), { std::string("One"), std::string("Two") } };
    window->set_randomizer_settings(settings);

    IWaypoint::WaypointRandomizerSettings new_settings;

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
    window->set_route(&route);
    window->select_waypoint(0);

    auto test1 = randomizer_group->find<ui::Dropdown>("test1");
    ASSERT_NE(test1, nullptr);
    ASSERT_EQ(test1->selected_value(), L"One");

    auto dropdown_button = test1->find<ui::Button>(ui::Dropdown::Names::dropdown_button);
    ASSERT_NE(dropdown_button, nullptr);
    dropdown_button->clicked(Point());

    auto dropdown_list = test1->dropdown_listbox();
    ASSERT_NE(dropdown_list, nullptr);

    auto row = dropdown_list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Name");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    test1->set_selected_value(L"Two");
    ASSERT_NE(new_settings.find("test1"), new_settings.end());
    ASSERT_EQ(std::get<std::string>(new_settings["test1"]), "Two");
}

TEST(RouteWindow, SetRandomizerTextUpdatesWaypoint)
{
    auto window = register_test_module().build();
    auto randomizer_group = window->root_control()->find<ui::Control>(RouteWindow::Names::randomizer_group);

    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::String, std::string("One") };
    window->set_randomizer_settings(settings);

    IWaypoint::WaypointRandomizerSettings new_settings;

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
    window->set_route(&route);
    window->select_waypoint(0);

    auto test1 = randomizer_group->find<ui::TextArea>("test1");
    ASSERT_NE(test1, nullptr);
    ASSERT_EQ(test1->text(), L"One");

    test1->set_text(L"Two");

    ASSERT_NE(new_settings.find("test1"), new_settings.end());
    ASSERT_EQ(std::get<std::string>(new_settings["test1"]), "Two");
}

TEST(RouteWindow, SetRandomizerNumberUpdatesWaypoint)
{
    auto window = register_test_module().build();
    auto randomizer_group = window->root_control()->find<ui::Control>(RouteWindow::Names::randomizer_group);

    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Number, 1.0f };
    window->set_randomizer_settings(settings);

    IWaypoint::WaypointRandomizerSettings new_settings;

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
    window->set_route(&route);
    window->select_waypoint(0);

    auto test1 = randomizer_group->find<ui::TextArea>("test1");
    ASSERT_NE(test1, nullptr);
    ASSERT_EQ(test1->text(), L"1.000000");

    test1->set_text(L"2.0");
    test1->gained_focus();
    test1->key_char(0xD);

    ASSERT_NE(new_settings.find("test1"), new_settings.end());
    ASSERT_EQ(std::get<float>(new_settings["test1"]), 2.0);
}

TEST(RouteWindow, SetRandomizerNumberUpdatesWaypointByChangingFocus)
{
    auto window = register_test_module().build();
    auto randomizer_group = window->root_control()->find<ui::Control>(RouteWindow::Names::randomizer_group);

    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Number, 1.0f };
    window->set_randomizer_settings(settings);

    IWaypoint::WaypointRandomizerSettings new_settings;

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(waypoint, set_randomizer_settings(An<const IWaypoint::WaypointRandomizerSettings&>())).WillRepeatedly(SaveArg<0>(&new_settings));
    window->set_route(&route);
    window->select_waypoint(0);

    auto test1 = randomizer_group->find<ui::TextArea>("test1");
    ASSERT_NE(test1, nullptr);
    ASSERT_EQ(test1->text(), L"1.000000");

    test1->gained_focus();
    test1->set_text(L"2.0");
    test1->lost_focus(nullptr);

    ASSERT_NE(new_settings.find("test1"), new_settings.end());
    ASSERT_EQ(std::get<float>(new_settings["test1"]), 2.0);
}

TEST(RouteWindow, RandomizerTextAreasUseShell)
{
    auto shell = std::make_shared<MockShell>();
    EXPECT_CALL(*shell, open(std::wstring(L"http://example.com"))).Times(1);

    auto window = register_test_module().with_shell(shell).build();

    RandomizerSettings settings;
    settings.settings["test_text"] = { "test_text", RandomizerSettings::Setting::Type::String };

    window->set_randomizer_enabled(true);
    window->set_randomizer_settings(settings);

    auto text_area = window->root_control()->find<TextArea>("test_text");
    ASSERT_NE(text_area, nullptr);

    text_area->set_text(L"http://example.com");
    text_area->clicked(Point(5, 5));
}
*/