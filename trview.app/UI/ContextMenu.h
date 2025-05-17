#pragma once

#include "IContextMenu.h"

namespace trview
{
    struct IItemsWindowManager;
    struct IPlugins;

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
            static inline const std::string filter = "Filter";
        };

        explicit ContextMenu(const std::weak_ptr<IItemsWindowManager>& items_window_manager, const std::weak_ptr<IPlugins>& plugins);
        virtual ~ContextMenu() = default;
        void render(const PickResult& pick_result) override;
        virtual void set_visible(bool value) override;
        virtual void set_remove_enabled(bool value) override;
        virtual void set_hide_enabled(bool value) override;
        virtual void set_mid_waypoint_enabled(bool value) override;
        virtual bool visible() const override;
        virtual void set_triggered_by(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        void set_tile_filter_enabled(bool value) override;
    private:
        bool _remove_enabled{ false };
        bool _hide_enabled{ false };
        bool _mid_enabled{ false };
        bool _can_show{ false };
        bool _visible{ false };
        bool _tile_filter_enabled{ false };
        std::vector<std::weak_ptr<ITrigger>> _triggered_by;
        std::weak_ptr<IItemsWindowManager> _items_window_manager;
        std::weak_ptr<IPlugins> _plugins;
    };
}
