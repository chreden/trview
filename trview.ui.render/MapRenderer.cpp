#include "MapRenderer.h"
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/ViewportStore.h>
#include <trview.graphics/SpriteSizeStore.h>

using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            MapRenderer::MapRenderer(const ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, int width, int height)
                : _device(device),
                _window_width(width), 
                _window_height(height),
                _sprite(device, shader_storage, width, height)
            {
                TextureStorage texture_storage{ device };
                _texture = texture_storage.coloured(0xFFFFFFFF);
            }

            void
            MapRenderer::render(const ComPtr<ID3D11DeviceContext>& context)
            {
                if (!_render_target)
                {
                    return;
                }

                if (needs_redraw())
                {
                    // Clear the render target to be transparent (as it may not be using the entire area).
                    _render_target->clear(context, Color(1, 1, 1, 1));

                    graphics::RenderTargetStore rs_store(context);
                    graphics::ViewportStore vp_store(context);
                    // Set the host size to match the render target as we will have adjusted the viewport.
                    graphics::SpriteSizeStore s_store(_sprite, _render_target->width(), _render_target->height());

                    _render_target->apply(context);
                    render_internal(context);
                }

                // Now render the render target in the correct position.
                auto p = Point(_first.x - 1, _first.y - 1);
                _sprite.render(context, _render_target->texture(), p.x, p.y, static_cast<float>(_render_target->width()), static_cast<float>(_render_target->height()));
            }

            void
            MapRenderer::render_internal(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
            {
                // Draw base square, this is the backdrop for the map 
                draw(context, Point(), Size(static_cast<float>(_render_target->width()), static_cast<float>(_render_target->height())), Color(0.0f, 0.0f, 0.0f));

                std::for_each(_tiles.begin(), _tiles.end(), [&](const Tile &tile)
                {
                    // Firstly get the colour for the tile 
                    // To determine the base colour we order the floor functions by the *minimum* enabled flag (ranked by order asc)
                    int minimum_flag_enabled = -1;
                    Color draw_color = Color(0.0f, 0.7f, 0.7f); // fallback 

                    for (const auto& color : default_colours)
                    {
                        if ((color.first & tile.sector->flags)
                            && (color.first < minimum_flag_enabled || minimum_flag_enabled == -1)
                            && (color.first < SectorFlag::ClimbableUp || color.first > SectorFlag::ClimbableLeft)) // climbable flag handled separately
                        {
                            minimum_flag_enabled = color.first;
                            draw_color = color.second;
                        }
                    }

                    // If the cursor is over the tile, then negate colour 
                    Point first = tile.position, last = Point(tile.size.width, tile.size.height) + tile.position;
                    if (_cursor.is_between(first, last))
                        draw_color.Negate();

                    // Draw the base tile 
                    draw(context, tile.position, tile.size, draw_color);

                    // Draw climbable walls. This draws 4 separate lines - one per climbable edge. 
                    // In the future I'd like to just draw a hollow square instead.
                    const float thickness = _DRAW_SCALE / 4;

                    if (tile.sector->flags & SectorFlag::ClimbableUp)
                        draw(context, tile.position, Size(tile.size.width, thickness), default_colours[SectorFlag::ClimbableUp]);
                    if (tile.sector->flags & SectorFlag::ClimbableRight)
                        draw(context, Point(tile.position.x + _DRAW_SCALE - thickness, tile.position.y), Size(thickness, tile.size.height), default_colours[SectorFlag::ClimbableRight]);
                    if (tile.sector->flags & SectorFlag::ClimbableDown)
                        draw(context, Point(tile.position.x, tile.position.y + _DRAW_SCALE - thickness), Size(tile.size.width, thickness), default_colours[SectorFlag::ClimbableDown]);
                    if (tile.sector->flags & SectorFlag::ClimbableLeft)
                        draw(context, tile.position, Size(thickness, tile.size.height), default_colours[SectorFlag::ClimbableLeft]);

                    // If sector is a down portal, draw a transparent black square over it 
                    if (tile.sector->flags & SectorFlag::RoomBelow)
                        draw(context, tile.position, tile.size, Color(0.0f, 0.0f, 0.0f, 0.6f));

                    // If sector is an up portal, draw a small corner square in the top left to signify this 
                    if (tile.sector->flags & SectorFlag::RoomAbove)
                        draw(context, tile.position, Size(tile.size.width / 4, tile.size.height / 4), Color(0.0f, 0.0f, 0.0f));
                });
            }

            void 
            MapRenderer::draw(const ComPtr<ID3D11DeviceContext>& context, Point p, Size s, Color c)
            {
                _sprite.render(context, _texture, p.x, p.y, s.width, s.height, c); 
            }

            void
            MapRenderer::load(trview::Room *room)
            {
                // Set window position and size 
                _columns = room->num_x_sectors();
                _rows = room->num_z_sectors();
                _loaded = true; 
                update_map_position(); 
                update_map_render_target();

                // Load up sectors 
                _tiles.clear(); 

                const auto& sectors = room->sectors(); 
                std::for_each(sectors.begin(), sectors.end(), [&] (const auto& pair) {
                    _tiles.emplace_back(std::shared_ptr<Sector>(pair.second), get_position(*pair.second), get_size());
                });
            }

            Point MapRenderer::get_position(const Sector& sector)
            {
                return Point {
                    /* X */ _DRAW_SCALE * (sector.id() / _rows),
                    /* Y */ _DRAW_SCALE * (_rows - (sector.id() % _rows) - 1)
                } + Point(1,1); 
            }

            Size MapRenderer::get_size() const
            {
                return Size { _DRAW_SCALE - 1, _DRAW_SCALE - 1 };
            }

            std::shared_ptr<Sector> 
            MapRenderer::sector_at(const Point& p) const
            {
                auto iter = std::find_if(_tiles.begin(), _tiles.end(), [&] (const Tile& tile) {
                    // Get bottom-right point of the tile 
                    Point last = Point(tile.size.width, tile.size.height) + tile.position; 
                    // Check if point is between the origin, and the bottom-right corner 
                    return p.is_between(tile.position, last); 
                });
                
                if (iter == _tiles.end())
                    return nullptr;
                else
                    return std::shared_ptr<Sector>(iter->sector);
            }

            std::shared_ptr<Sector> 
            MapRenderer::sector_at_cursor() const
            {
                return sector_at(_cursor);
            }

            bool 
            MapRenderer::cursor_is_over_control() const
            {
                return _render_target && _cursor.is_between(Point(), Point(static_cast<float>(_render_target->width()), static_cast<float>(_render_target->height())));
            }

            void 
            MapRenderer::set_window_size(int width, int height)
            {
                _window_width = width;
                _window_height = height;
                _sprite.set_host_size(width, height);
                update_map_position();
            }

            void 
            MapRenderer::update_map_position()
            {
                // Location of the origin of the control 
                _first = Point(_window_width - (_DRAW_SCALE * _columns) - _DRAW_MARGIN, _DRAW_MARGIN);
                // Location of the last point of the control (bottom-right)
                _last = _first + Point(_DRAW_SCALE * _columns, _DRAW_SCALE * _rows);
            }

            void
            MapRenderer::update_map_render_target()
            {
                uint32_t width = static_cast<uint32_t>(_DRAW_SCALE * _columns + 1);
                uint32_t height = static_cast<uint32_t>(_DRAW_SCALE * _rows + 1);

                // Minor optimisation - don't recreate the render target if the room dimensions are the same.
                if (!_render_target || (_render_target->width() != width || _render_target->height() != height))
                {
                    _render_target = std::make_unique<graphics::RenderTarget>(_device, width, height);
                }
                _force_redraw = true;
            }

            bool 
            MapRenderer::needs_redraw()
            {
                if (_force_redraw)
                {
                    return true;
                }

                if (cursor_is_over_control())
                {
                    _cursor_was_over = true;
                    return true;
                }

                if (_cursor_was_over)
                {
                    _cursor_was_over = false;
                    return true;
                }

                return false;
            }
        }
    }
};