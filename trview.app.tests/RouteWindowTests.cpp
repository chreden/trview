#include <trview.app/Windows/RouteWindow.h>
#include <trview.ui.render/Mocks/IRenderer.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.ui/Mocks/Input/IInput.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.app/Mocks/UI/IBubble.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>

using namespace DirectX::SimpleMath;
using namespace testing;
using namespace trview;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::mocks;
using namespace trview::tests;
using namespace trview::ui::mocks;
using namespace trview::ui::render::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IDeviceWindow::Source device_window_source{ [](auto&&...) { return std::make_unique<MockDeviceWindow>(); } };
            ui::render::IRenderer::Source renderer_source{ [](auto&&...) { return std::make_unique<MockRenderer>(); } };
            ui::IInput::Source input_source{ [](auto&&...) { return std::make_unique<MockInput>(); } };
            Window parent{ create_test_window(L"RouteWindowTests") };
            std::shared_ptr<IClipboard> clipboard{ std::make_shared<MockClipboard>() };
            std::shared_ptr<IDialogs> dialogs{ std::make_shared<MockDialogs>() };
            std::shared_ptr<IFiles> files{ std::make_shared<MockFiles>() };
            IBubble::Source bubble_source{ [](auto&&...) { return std::make_unique<MockBubble>(); }};

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

            test_module& with_bubble_source(const IBubble::Source& source)
            {
                this->bubble_source = source;
                return *this;
            }

            std::unique_ptr<RouteWindow> build()
            {
                return std::make_unique<RouteWindow>(device_window_source, renderer_source, input_source,
                    parent, clipboard, dialogs, files, bubble_source);
            }
        };
        return test_module{};
    }
}

TEST(RouteWindow, WaypointRoomPositionCalculatedCorrectly)
{
    const Vector3 room_pos{ 102400, 204800, 307200 };
    const Vector3 waypoint_pos{ 130, 250, 325 };
    const Vector3 expected{ 30720, 51200, 25600 };

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
    auto waypoint_stats = window->root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
    ASSERT_NE(waypoint_stats, nullptr);

    auto items = waypoint_stats->items();
    auto item_iter = std::find_if(items.begin(), items.end(), [](auto i) { return i.value(L"Name") == L"Room Position"; });
    ASSERT_NE(item_iter, items.end());

    auto value = item_iter->value(L"Value");
    value.erase(std::remove_if(value.begin(), value.end(),
        [](wchar_t c)
        {
            return std::isspace(static_cast<int>(c));
        }), value.end());
    std::replace(value.begin(), value.end(), L',', L' ');

    int32_t x, y, z;
    std::wstringstream stream(value);
    stream >> x >> y >> z;

    ASSERT_EQ(x, expected.x);
    ASSERT_EQ(y, expected.y);
    ASSERT_EQ(z, expected.z);
}

TEST(RouteWindow, PositionValuesCopiedToClipboard)
{
    auto clipboard = std::make_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write(An<const Window&>(), std::wstring(L"133120, 256000, 332800"))).Times(1);

    auto window = register_test_module().with_clipboard(clipboard).build();

    const Vector3 waypoint_pos{ 130, 250, 325 };
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, position).WillRepeatedly(Return(waypoint_pos));

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_route(&route);
    window->select_waypoint(0);

    auto waypoint_stats = window->root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
    ASSERT_NE(waypoint_stats, nullptr);

    auto row = waypoint_stats->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());
}

TEST(RouteWindow, RoomPositionValuesCopiedToClipboard)
{
    auto clipboard = std::make_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write(An<const Window&>(), std::wstring(L"133120, 256000, 332800"))).Times(1);

    auto window = register_test_module().with_clipboard(clipboard).build();

    const Vector3 waypoint_pos{ 130, 250, 325 };
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, position).WillRepeatedly(Return(waypoint_pos));

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));

    window->set_route(&route);
    window->select_waypoint(0);

    auto waypoint_stats = window->root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
    ASSERT_NE(waypoint_stats, nullptr);

    auto row = waypoint_stats->find<ui::Control>(ui::Listbox::Names::row_name_format + "3");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());
}

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

    auto notes_area = window->root_control()->find<ui::TextArea>(RouteWindow::Names::notes_area);
    ASSERT_NE(notes_area, nullptr);

    notes_area->on_text_changed(L"Test");
}

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

    auto clear_save = window->root_control()->find<ui::Button>(RouteWindow::Names::clear_save);
    ASSERT_NE(clear_save, nullptr);

    clear_save->on_click();
}

TEST(RouteWindow, ExportRouteButtonRaisesEvent)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::SaveFileResult{ "filename", 0 }));

    std::optional<std::string> file_raised;
    auto window = register_test_module().with_dialogs(dialogs).build();
    auto token = window->on_route_export += [&](const auto& filename, const auto& is_rando)
    {
        file_raised = filename;
    };

    auto export_button = window->root_control()->find<ui::Button>(RouteWindow::Names::export_button);
    ASSERT_NE(export_button, nullptr);

    export_button->on_click();

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
    auto export_button = window->root_control()->find<ui::Button>(RouteWindow::Names::export_button);
    ASSERT_NE(export_button, nullptr);

    export_button->on_click();
    ASSERT_FALSE(file_raised);
}

TEST(RouteWindow, ImportRouteButtonRaisesEvent)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return("filename"));

    std::optional<std::string> file_raised;
    auto window = register_test_module().with_dialogs(dialogs).build();
    auto token = window->on_route_import += [&](const auto& filename)
    {
        file_raised = filename;
    };

    auto import_button = window->root_control()->find<ui::Button>(RouteWindow::Names::import_button);
    ASSERT_NE(import_button, nullptr);

    import_button->on_click();

    ASSERT_TRUE(file_raised.has_value());
    ASSERT_EQ(file_raised, "filename");
}

TEST(RouteWindow, ImportRouteButtonDoesNotRaiseEventWhenCancelled)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1);

    bool file_raised = false;
    auto window = register_test_module().with_dialogs(dialogs).build();
    auto token = window->on_route_import += [&](const auto& filename)
    {
        file_raised = true;
    };
    auto import_button = window->root_control()->find<ui::Button>(RouteWindow::Names::import_button);
    ASSERT_NE(import_button, nullptr);

    import_button->on_click();
    ASSERT_FALSE(file_raised);
}

TEST(RouteWindow, ExportSaveButtonSavesFile)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::SaveFileResult{ "filename", 0 }));

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

    auto export_save_button = window->root_control()->find<ui::Button>(RouteWindow::Names::select_save_button);
    ASSERT_NE(export_save_button, nullptr);

    export_save_button->on_click();
}

TEST(RouteWindow, ExportSaveButtonShowsErrorOnFailure)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::SaveFileResult{ "filename", 0 }));
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

    auto export_save_button = window->root_control()->find<ui::Button>(RouteWindow::Names::select_save_button);
    ASSERT_NE(export_save_button, nullptr);

    export_save_button->on_click();
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

    auto export_save_button = window->root_control()->find<ui::Button>(RouteWindow::Names::select_save_button);
    ASSERT_NE(export_save_button, nullptr);

    export_save_button->on_click();
}

TEST(RouteWindow, AttachSaveButtonLoadsSave)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return("filename"));

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

    auto attach_save_button = window->root_control()->find<ui::Button>(RouteWindow::Names::select_save_button);
    ASSERT_NE(attach_save_button, nullptr);

    attach_save_button->on_click();
}

TEST(RouteWindow, AttachSaveButtonShowsMessageOnError)
{
    auto dialogs = std::make_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return("filename"));
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

    auto attach_save_button = window->root_control()->find<ui::Button>(RouteWindow::Names::select_save_button);
    ASSERT_NE(attach_save_button, nullptr);

    attach_save_button->on_click();

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

    auto attach_save_button = window->root_control()->find<ui::Button>(RouteWindow::Names::select_save_button);
    ASSERT_NE(attach_save_button, nullptr);

    attach_save_button->on_click();
    ASSERT_FALSE(waypoint.has_save());
}

TEST(RouteWindow, ClickStatShowsBubble)
{
    auto bubble = std::make_unique<MockBubble>();
    EXPECT_CALL(*bubble, show(testing::A<const Point&>())).Times(1);

    auto window = register_test_module().with_bubble_source([&](auto&&...) { return std::move(bubble); }).build();

    MockWaypoint waypoint;
    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    window->set_route(&route);
    window->select_waypoint(0);

    auto stats = window->root_control()->find<ui::Listbox>(RouteWindow::Names::waypoint_stats);
    ASSERT_NE(stats, nullptr);

    auto first_stat = stats->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(first_stat, nullptr);

    auto value = first_stat->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(value, nullptr);
    value->clicked(Point());
}

TEST(RouteWindow, RequiresGlitchSetRouteUnsaved)
{
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, requires_glitch).Times(AtLeast(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(waypoint, type).WillRepeatedly(Return(IWaypoint::Type::RandoLocation));
    EXPECT_CALL(waypoint, set_requires_glitch(false)).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    auto requires_glitch = window->root_control()->find<ui::Checkbox>(RouteWindow::Names::requires_glitch);
    ASSERT_NE(requires_glitch, nullptr);
    ASSERT_TRUE(requires_glitch->visible(true));

    requires_glitch->clicked(Point());
}

TEST(RouteWindow, IsInRoomSpaceSetsRouteUnsaved)
{
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, is_in_room_space).Times(AtLeast(1)).WillRepeatedly(Return(false));
    EXPECT_CALL(waypoint, type).WillRepeatedly(Return(IWaypoint::Type::RandoLocation));
    EXPECT_CALL(waypoint, set_is_in_room_space(true)).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    auto is_in_room_space = window->root_control()->find<ui::Checkbox>(RouteWindow::Names::is_in_room_space);
    ASSERT_NE(is_in_room_space, nullptr);
    ASSERT_TRUE(is_in_room_space->visible(true));

    is_in_room_space->clicked(Point());
}

TEST(RouteWindow, VehicleRequiredSetsRouteUnsaved)
{
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, vehicle_required).Times(AtLeast(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(waypoint, type).WillRepeatedly(Return(IWaypoint::Type::RandoLocation));
    EXPECT_CALL(waypoint, set_vehicle_required(false)).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    auto vehicle_required = window->root_control()->find<ui::Checkbox>(RouteWindow::Names::vehicle_required);
    ASSERT_NE(vehicle_required, nullptr);
    ASSERT_TRUE(vehicle_required->visible(true));

    vehicle_required->clicked(Point());
}

TEST(RouteWindow, IsItemSetsRouteUnsaved)
{
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, is_item).Times(AtLeast(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(waypoint, type).WillRepeatedly(Return(IWaypoint::Type::RandoLocation));
    EXPECT_CALL(waypoint, set_is_item(false)).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    auto is_item = window->root_control()->find<ui::Checkbox>(RouteWindow::Names::is_item);
    ASSERT_NE(is_item, nullptr);
    ASSERT_TRUE(is_item->visible(true));

    is_item->clicked(Point());
}

TEST(RouteWindow, DifficultySetsRouteUnsaved)
{
    MockWaypoint waypoint;
    EXPECT_CALL(waypoint, difficulty).Times(AtLeast(1)).WillRepeatedly(Return("Medium"));
    EXPECT_CALL(waypoint, type).WillRepeatedly(Return(IWaypoint::Type::RandoLocation));
    EXPECT_CALL(waypoint, set_difficulty("Hard")).Times(1);

    MockRoute route;
    EXPECT_CALL(route, waypoints).WillRepeatedly(Return(1));
    EXPECT_CALL(route, waypoint(An<uint32_t>())).WillRepeatedly(ReturnRef(waypoint));
    EXPECT_CALL(route, set_unsaved(true)).Times(1);

    auto window = register_test_module().build();
    window->set_route(&route);

    auto difficulty = window->root_control()->find<ui::Dropdown>(RouteWindow::Names::difficulty);
    ASSERT_NE(difficulty, nullptr);
    ASSERT_TRUE(difficulty->visible(true));
    
    auto dropdown_button = difficulty->find<ui::Button>(ui::Dropdown::Names::dropdown_button);
    ASSERT_NE(dropdown_button, nullptr);
    dropdown_button->clicked(Point());

    auto dropdown_list = difficulty->dropdown_listbox();
    ASSERT_NE(dropdown_list, nullptr);

    auto row = dropdown_list->find<ui::Control>(ui::Listbox::Names::row_name_format + "2");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Name");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());
}


