#include "pch.h"
#include "trview_tests.h"
#include "CameraControlsTests.h"
#include "CameraPositionTests.h"
#include "CameraSinkWindowTests.h"
#include "ConsoleTests.h"
#include "ContextMenuTests.h"
#include "FiltersTests.h"
#include "GoToTests.h"
#include "ItemsWindowTests.h"
#include "LevelInfoTests.h"
#include "LightsWindowTests.h"
#include "LogWindowTests.h"
#include "PluginsWindowTests.h"
#include "RoomNavigatorTests.h"
#include "RoomsWindowTests.h"
#include "RouteWindowTests.h"
#include "SettingsWindowTests.h"
#include "StaticsWindowTests.h"
#include "TexturesWindowTests.h"
#include "TriggersWindowTests.h"
#include "ViewOptionsTests.h"

void register_trview_tests(ImGuiTestEngine* engine)
{
    register_camera_controls_tests(engine);
    register_camera_position_tests(engine);
    register_camera_sink_window_tests(engine);
    register_console_tests(engine);
    register_context_menu_tests(engine);
    register_filters_tests(engine);
    register_go_to_tests(engine);
    register_items_window_tests(engine);
    register_level_info_tests(engine);
    register_lights_window_tests(engine);
    register_log_window_tests(engine);
    register_plugins_window_tests(engine);
    register_room_navigator_controls_tests(engine);
    register_rooms_window_tests(engine);
    register_route_window_tests(engine);
    register_settings_window_tests(engine);
    register_statics_window_tests(engine);
    register_textures_window_tests(engine);
    register_triggers_window_tests(engine);
    register_view_options_tests(engine);
}
