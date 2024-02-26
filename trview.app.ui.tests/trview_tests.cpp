#include "trview_tests.h"
#include "ViewOptionsTests.h"
#include "CameraControlsTests.h"

void register_trview_tests(ImGuiTestEngine* engine)
{
    register_camera_controls_tests(engine);
    register_view_options_tests(engine);
}
