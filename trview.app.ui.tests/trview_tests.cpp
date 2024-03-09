#include "pch.h"
#include "trview_tests.h"
#include "CameraControlsTests.h"
#include "CameraPositionTests.h"
#include "ContextMenuTests.h"
#include "GoToTests.h"
#include "ItemsWindowTests.h"
#include "LevelInfoTests.h"
#include "RoomNavigatorTests.h"
#include "SettingsWindowTests.h"
#include "ViewOptionsTests.h"

void register_trview_tests(ImGuiTestEngine* engine)
{
    register_camera_controls_tests(engine);
    register_camera_position_tests(engine);
    register_context_menu_tests(engine);
    register_go_to_tests(engine);
    register_items_window_tests(engine);
    register_level_info_tests(engine);
    register_room_navigator_controls_tests(engine);
    register_settings_window_tests(engine);
    register_view_options_tests(engine);
}
