#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

#include <vector> 
#include <memory>
#include <algorithm>
#include <map>

#include <trview.graphics/Sprite.h>
#include "trview\Types.h"
#include "trview\TextureStorage.h"
#include <trview.graphics/Texture.h>
#include <trview.ui/Point.h>
#include <trview.common/Size.h>
#include "trview\Room.h"
#include <trview.graphics/RenderTarget.h>

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
                    ui::Point position; 
                    Size size; 
                };

                std::map<SectorFlag, DirectX::SimpleMath::Color> default_colours = {
                    { Portal, { 0.0f, 0.0f, 0.0f } }, 
                    { Wall, { 0.4f, 0.4f, 0.4f } }, 
                    { Trigger, { 1.0f, 0.3f, 0.7f } },
                    { Death, { 0.9f, 0.1f, 0.1f } },
                    { MinecartLeft, { 0.0f, 0.9f, 0.9f } },
                    { MinecartRight, { 0.0f, 0.9f, 0.9f } },
                    { MonkeySwing, { 0.9f, 0.9f, 0.4f } },
                    { ClimbableUp, { 0.0f, 0.9f, 0.0f, 0.6f } },
                    { ClimbableDown, { 0.0f, 0.9f, 0.0f, 0.6f } },
                    { ClimbableRight, { 0.0f, 0.9f, 0.0f, 0.6f } },
                    { ClimbableLeft, { 0.0f, 0.9f, 0.0f, 0.6f } },
                };
            }

            class MapRenderer
            {
            public:
                MapRenderer(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, int width, int height);

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
                inline void set_cursor_position(const Point& cursor) { _cursor = cursor - _first; }

                // Returns whether the map is loaded
                inline bool loaded() const { return _loaded; }

                // Set the size of the host window.
                void set_window_size(int width, int height);
            private:
                // Determines the position (on screen) to draw a sector 
                ui::Point get_position(const Sector& sector); 

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

                Microsoft::WRL::ComPtr<ID3D11Device>               _device;
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
                const float                         _DRAW_SCALE = 14.0f; 
            };
        }
    }
};
