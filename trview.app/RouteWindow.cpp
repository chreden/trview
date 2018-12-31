#include "RouteWindow.h"

namespace trview
{
    namespace Colours
    {
        const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
        const Colour LeftPanel{ 1.0f, 0.4f, 0.4f, 0.4f };
        const Colour ItemDetails{ 1.0f, 0.35f, 0.35f, 0.35f };
        const Colour Triggers{ 1.0f, 0.3f, 0.3f, 0.3f };
        const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
    }

    using namespace graphics;

    RouteWindow::RouteWindow(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.route", L"Route", Size(470, 400))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    std::unique_ptr<ui::Control> RouteWindow::create_left_panel()
    {
        using namespace ui;
        auto left_panel = std::make_unique<ui::StackPanel>(Point(), Size(200, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);
        return left_panel;
    }

    std::unique_ptr<ui::Control> RouteWindow::create_right_panel()
    {
        using namespace ui;
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Point(), Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        return right_panel;
    }
}
