#pragma once

#include "IContextMenu.h"

namespace trview
{
    class ContextMenu final : public IContextMenu
    {
    public:
        /// Control names used for automation
        struct Names
        {
            static inline const std::string add_waypoint = "Add Waypoint";
            static const std::string add_mid_waypoint_button;
            static inline const std::string hide = "Hide";
            static const std::string orbit_button;
            static const std::string remove_waypoint_button;
        };

        virtual ~ContextMenu() = default;
        virtual void render() override;
        virtual void set_visible(bool value) override;
        virtual void set_remove_enabled(bool value) override;
        virtual void set_hide_enabled(bool value) override;
        virtual void set_mid_waypoint_enabled(bool value) override;
        virtual bool visible() const override;
    private:
        bool _remove_enabled{ false };
        bool _hide_enabled{ false };
        bool _mid_enabled{ false };
        bool _can_show{ false };
        bool _visible{ false };
    };
}
