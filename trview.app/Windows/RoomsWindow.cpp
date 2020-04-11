#include "RoomsWindow.h"
#include <trview.ui/StackPanel.h>

namespace trview
{
    namespace
    {
        namespace Colours
        {
            const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour LeftPanel{ 1.0f, 0.25f, 0.25f, 0.25f };
            const Colour ItemDetails{ 1.0f, 0.225f, 0.225f, 0.225f };
            const Colour Triggers{ 1.0f, 0.20f, 0.20f, 0.20f };
            const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
        }
    }

    RoomsWindow::RoomsWindow(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const Window& parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.rooms", L"Rooms", Size(470, 400))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_left_panel()
    {
        using namespace ui;
        auto left_panel = std::make_unique<ui::StackPanel>(Size(200, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);
        return left_panel;
    }
    
    std::unique_ptr<ui::Control> RoomsWindow::create_right_panel()
    {
        using namespace ui;
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        return right_panel;
    }
}