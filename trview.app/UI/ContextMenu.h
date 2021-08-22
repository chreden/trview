#pragma once

#include "IContextMenu.h"
#include <trview.ui/Button.h>

namespace trview
{
    class ContextMenu final : public IContextMenu
    {
    public:
        /// Control names used for automation
        struct Names
        {
            static const std::string add_waypoint_button;
            static const std::string add_mid_waypoint_button;
            static const std::string hide_button;
            static const std::string orbit_button;
            static const std::string remove_waypoint_button;
        };

        /// Create a new ContentMenu window.
        /// @param parent The control to add the window to.
        explicit ContextMenu(ui::Control& parent);
        virtual ~ContextMenu() = default;
        /// Get the root control.
        /// @returns The root control.
        const ui::Control* control() const;
        virtual void set_position(const Point& position) override;
        virtual void set_visible(bool value) override;
        virtual void set_remove_enabled(bool value) override;
        virtual void set_hide_enabled(bool value) override;
        virtual bool visible() const override;
        virtual void set_mid_waypoint_enabled(bool value) override;
    private:
        ui::Window* _menu;
        ui::Button* _remove_button;
        ui::Button* _hide_button;
        ui::Button* _mid_button;
        bool _remove_enabled{ false };
        bool _hide_enabled{ false };
        bool _mid_enabled{ false };
        TokenStore _token_store;
    };
}
