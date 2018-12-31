#pragma once

#include <trview.ui/Listbox.h>
#include "CollapsiblePanel.h"

namespace trview
{
    class RouteWindow final : public CollapsiblePanel
    {
    public:
        /// Create an items window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit RouteWindow(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, HWND parent);

        /// Destructor for triggers window
        virtual ~RouteWindow() = default;
    private:
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
    };
}
