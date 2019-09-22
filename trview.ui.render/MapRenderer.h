#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

#include <vector> 
#include <memory>
#include <algorithm>
#include <map>

#include <trview.graphics/Sprite.h>
#include <trview.app/Elements/Types.h>
#include <trview.graphics/Texture.h>
#include <trview.common/Point.h>
#include <trview.common/Size.h>
#include <trview.app/Elements/Room.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.common/Event.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/Font.h>

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
    }

    namespace ui
    {
        namespace render
        {
            namespace
            {
                struct Tile
                {
                public:
                    Tile(const std::shared_ptr<Sector>& p_sector, Point p_position, Size p_size)
                        : sector(p_sector), position(p_position), size(p_size) {}

                    std::shared_ptr<Sector> sector; 
                    Point position; 
                    Size size; 
                };
            }

            class MapRenderer
            {
            public:
                MapRenderer(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const Size& window_size);

                // Renders the map 
                void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

                // Changes the room to specified room, reloads map
                void load(trview::Room *room);

                // Returns the total area of the room 
                inline std::uint16_t area() const { return _columns * _rows; }

                // Returns the sector under the specified position, or nullptr if none
                std::shared_ptr<Sector> sector_at(const Point& p) const;

                // Returns the sector that the cursor is within, or nullptr if none
                std::shared_ptr<Sector> sector_at_cursor() const;

                // Returns true if cursor is on the control
                bool cursor_is_over_control() const;

                // Sets the position of the cursor 
                void set_cursor_position(const Point& cursor);

                // Returns whether the map is loaded
                inline bool loaded() const { return _loaded; }

                // Set the size of the host window.
                void set_window_size(const Size& size);

                /// Set whether the map is visible.
                void set_visible(bool visible);

                void clear_highlight();

                void set_highlight(uint16_t x, uint16_t z);

                /// Event raised when the user hovers over a map sector, or if the mouse leaves the map.
                Event<std::shared_ptr<Sector>> on_sector_hover;
            private:
                // Determines the position (on screen) to draw a sector 
                Point get_position(const Sector& sector); 

                // Determines the size of a sector 
                Size get_size() const;

                // Draws a square at given position, size with given colour.
                void draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, Point p, Size s, DirectX::SimpleMath::Color c);

                // Update the stored positions of the corners of the map.
                void update_map_position();

                // Update the render target that the squares will be rendered to, depending
                // on the size of the room (based on columns and rows).
                void update_map_render_target();

                // Render the map squares and the background.
                void render_internal(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

                // Determines if the minimap needs to be re-drawn.
                bool needs_redraw();

                const graphics::Device&                            _device;
                bool                                               _visible = true;
                int                                                _window_width, _window_height;
                graphics::Sprite                                   _sprite; 
                graphics::Texture                                  _texture;
                std::vector<Tile>                                  _tiles; 
                std::unique_ptr<graphics::RenderTarget>            _render_target;

                Point                               _first, _last; // top-left corner, bottom-right corner (of control) 
                Point                               _cursor; // Position of the cursor relative to the top left of the control.
                std::uint16_t                       _rows, _columns; 
                bool                                _loaded = false;
                
                bool                                _cursor_was_over = false;
                bool                                _force_redraw = true;

                const float                         _DRAW_MARGIN = 30.0f; 
                const float                         _DRAW_SCALE = 17.0f; 

                std::optional<std::pair<uint16_t, uint16_t>> _selected_sector;
                std::unique_ptr<graphics::Font> _font;
                std::shared_ptr<Sector> _previous_sector;
                Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
            };
        }
    }
};
