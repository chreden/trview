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
            static inline const std::string add_mid_waypoint = "Add Mid Waypoint";
            static inline const std::string hide = "Hide";
            static inline const std::string orbit = "Orbit Here";
            static inline const std::string remove_waypoint = "Remove Waypoint";
            static inline const std::string copy = "Copy";
            static inline const std::string copy_position = "Position";
            static inline const std::string copy_number = "Room/Object Number";
            static inline const std::string trigger_references = "Trigger References";
        };

        virtual ~ContextMenu() = default;
        virtual void render() override;
        virtual void set_visible(bool value) override;
        virtual void set_remove_enabled(bool value) override;
        virtual void set_hide_enabled(bool value) override;
        virtual void set_mid_waypoint_enabled(bool value) override;
        virtual bool visible() const override;
        virtual void set_triggered_by(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
    private:
        bool _remove_enabled{ false };
        bool _hide_enabled{ false };
        bool _mid_enabled{ false };
        bool _can_show{ false };
        bool _visible{ false };
        std::vector<std::weak_ptr<ITrigger>> _triggered_by;
    };
}
