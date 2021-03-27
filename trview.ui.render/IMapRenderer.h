#pragma once

#include <trview.common/Event.h>
#include <trview.app/Elements/Room.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            struct IMapRenderer
            {
                using MapRendererSource = std::function<std::unique_ptr<IMapRenderer>(const Size&)>;

                virtual ~IMapRenderer() = 0;

                /// Event raised when the user hovers over a map sector, or if the mouse leaves the map.
                Event<std::shared_ptr<Sector>> on_sector_hover;

                // Renders the map 
                virtual void render(bool to_screen = true) = 0;

                // Changes the room to specified room, reloads map
                virtual void load(const trview::Room* room) = 0;

                // Returns the total area of the room 
                virtual std::uint16_t area() const = 0;

                // Returns the sector under the specified position, or nullptr if none
                virtual std::shared_ptr<Sector> sector_at(const Point& p) const = 0;

                // Returns the sector that the cursor is within, or nullptr if none
                virtual std::shared_ptr<Sector> sector_at_cursor() const = 0;

                // Returns true if cursor is on the control
                virtual bool cursor_is_over_control() const = 0;

                // Sets the position of the cursor 
                virtual void set_cursor_position(const Point& cursor) = 0;

                // Returns whether the map is loaded
                virtual bool loaded() const = 0;

                // Set the size of the host window.
                virtual void set_window_size(const Size& size) = 0;

                /// Set whether the map is visible.
                virtual void set_visible(bool visible) = 0;

                virtual void clear_highlight() = 0;

                virtual void set_highlight(uint16_t x, uint16_t z) = 0;

                virtual graphics::Texture texture() const = 0;

                virtual Point first() const = 0;
            };
        }
    }
}
