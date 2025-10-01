#pragma once

#include <trview.common/Event.h>
#include <trview.app/Elements/Room.h>
#include "../Settings/UserSettings.h"

namespace trview
{
    struct IMapRenderer
    {
        enum class Mode
        {
            Normal,
            Select
        };

        using Source = std::function<std::unique_ptr<IMapRenderer>()>;

        virtual ~IMapRenderer() = 0;

        /// Event raised when the user hovers over a map sector, or if the mouse leaves the map.
        Event<std::weak_ptr<IRoom>> on_room_selected;
        Event<std::weak_ptr<ISector>> on_sector_hover;
        Event<std::weak_ptr<ISector>> on_sector_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        // Renders the map 
        virtual void render(bool window) = 0;
        // Changes the room to specified room, reloads map
        virtual void load(const std::shared_ptr<trview::IRoom>& room) = 0;
        // Returns true if cursor is on the control
        virtual bool cursor_is_over_control() const = 0;
        // Returns whether the map is loaded
        virtual bool loaded() const = 0;
        /// Set whether the map is visible.
        virtual void set_visible(bool visible) = 0;
        virtual void clear_highlight() = 0;
        virtual void set_highlight(uint16_t x, uint16_t z) = 0;
        virtual void set_mode(Mode mode) = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
        virtual void set_selection(const std::shared_ptr<ISector>& sector) = 0;
        virtual void set_show_tooltip(bool value) = 0;
        virtual Size size() const = 0;
        virtual void reposition() = 0;
    };
}
