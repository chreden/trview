#include "LauWindow.h"
#include "../Resources/resource.h"

namespace trview
{
    ILauWindow::~ILauWindow()
    {
    }

    LauWindow::LauWindow(const graphics::IDeviceWindow::Source& device_window_source,
        const ui::render::IRenderer::Source& renderer_source,
        const ui::IInput::Source& input_source,
        const Window& parent,
        const std::shared_ptr<ui::ILoader>& loader)
        : CollapsiblePanel(device_window_source, renderer_source(Size(600, 600)), parent, L"trview.lau", L"LAU Development", input_source,
             Size(600, 600), loader->load_from_resource(IDR_UI_LAU_WINDOW))
    {
        bind_controls();
    }

    void LauWindow::render(bool vsync)
    {
        CollapsiblePanel::render(vsync);
    }

    void LauWindow::bind_controls()
    {
        using namespace ui;


    }
}
