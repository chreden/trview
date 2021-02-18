#include <trview.app/Windows/Viewer.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/IViewerUI.h>
#include <trview.common/Windows/Shortcuts.h>

using testing::NiceMock;
using testing::Return;
using namespace trview;
using namespace trview::graphics;
using namespace trview::tests;

TEST(Viewer, SelectItemRaised)
{
    std::unique_ptr<IViewerUI> ui_ptr = std::make_unique<mocks::MockViewerUI>();
    IViewerUI& ui = *ui_ptr;

    auto window = create_test_window(L"ViewerTests");

    Device device;
    ShaderStorage shader_storage;
    Shortcuts shortcuts(window);
    Route route(device, shader_storage);

    Viewer viewer(window, device, shader_storage, std::move(ui_ptr), shortcuts, &route);
    // viewer->open();

    auto token = viewer.on_item_selected += [](Item item)
    {

    };

    // ui.on_select_item(0);

    FAIL();
}