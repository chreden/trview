#include "pch.h"
#include "trview_tests.h"
#include "CameraControlsTests.h"
#include "RoomNavigatorTests.h"
#include "SettingsWindowTests.h"
#include "ViewOptionsTests.h"

void register_trview_tests(ImGuiTestEngine* engine)
{
    register_camera_controls_tests(engine);
    register_room_navigator_controls_tests(engine);
    register_settings_window_tests(engine);
    register_view_options_tests(engine);
}
