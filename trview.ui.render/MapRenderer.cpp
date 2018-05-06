#include "MapRenderer.h"

using namespace DirectX;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            MapRenderer::MapRenderer(CComPtr<ID3D11Device> device, int width, int height)
                : _device(device),
                _window_width(width), 
                _window_height(height),
                _sprite(device, width, height),
                _texture_storage(device)
            {
            }

            void
            MapRenderer::render(CComPtr<ID3D11DeviceContext> context)
            {
                // Draw base square, this is the backdrop for the map 
                draw(context, Point(_first.x - 1, _first.y - 1), Size(_DRAW_SCALE * _columns + 1, _DRAW_SCALE * _rows + 1), Color(0.0f, 0.0f, 0.0f));

                std::for_each(_tiles.begin(), _tiles.end(), [&] (const Tile &tile) 
                {
                    // Firstly get the colour for the tile 
                    // To determine the base colour we order the floor functions by the *minimum* enabled flag (ranked by order asc)
                    int minimum_flag_enabled = -1;
                    Color draw_color = Color(0.0f, 0.7f, 0.7f); // fallback 

                    for (const auto& color : default_colours)
                    {
                        if ((color.first & tile.sector->flags)
                            && (color.first < minimum_flag_enabled || minimum_flag_enabled == -1))
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

                    // If sector is a down portal, draw a transparent black square over it 
                    if (tile.sector->flags & SectorFlag::RoomBelow)
                        draw(context, tile.position, tile.size, Color(0.0f, 0.0f, 0.0f, 0.6f));

                    // If sector is an up portal, draw a small corner square in the top left to signify this 
                    if (tile.sector->flags & SectorFlag::RoomAbove)
                        draw(context, tile.position, Size(tile.size.width / 4, tile.size.height / 4), Color(0.0f, 0.0f, 0.0f)); 
                });
            }

            void 
            MapRenderer::draw(CComPtr<ID3D11DeviceContext> context, Point p, Size s, Color c)
            {
                _sprite.render(context, get_texture(), p.x, p.y, s.width, s.height, c); 
            }

            void
            MapRenderer::load(trview::Room *room)
            {
                // Set window position and size 
                _columns = room->num_x_sectors();
                _rows = room->num_z_sectors();
                _loaded = true; 
                update_map_position(); 

                // Load up sectors 
                _tiles.clear(); 

                const auto& sectors = room->sectors(); 
                std::for_each(sectors.begin(), sectors.end(), [&] (const auto& pair) {
                    _tiles.emplace_back(std::shared_ptr<Sector>(pair.second), get_position(*pair.second), get_size(*pair.second));
                });
            }

            Point MapRenderer::get_position(const Sector& sector)
            {
                return Point {
                    /* X */ _DRAW_SCALE * (sector.id() / _rows),
                    /* Y */ _DRAW_SCALE * (_rows - (sector.id() % _rows) - 1)
                } + _first; 
        
            }

            ui::Size MapRenderer::get_size(const Sector& sector) const
            {
                return ui::Size { _DRAW_SCALE - 1, _DRAW_SCALE - 1 };
            }

            CComPtr<ID3D11ShaderResourceView>
            MapRenderer::get_texture()
            {
                if (_texture == nullptr) _texture = _texture_storage.coloured(0xFFFFFFFF).view;
                return _texture;
            }

            std::shared_ptr<Sector> 
            MapRenderer::sector_at(const Point& p) const
            {
                std::shared_ptr<Sector> ptr = nullptr; 
                std::for_each(_tiles.begin(), _tiles.end(), [&] (const Tile& tile)
                {
                    Point first = tile.position;
                    Point last = Point(tile.size.width, tile.size.height) + tile.position;

                    if (p.is_between(first, last))
                        ptr = tile.sector; 
                });
                return ptr; 
            }

            std::shared_ptr<Sector> 
            MapRenderer::sector_at_cursor() const
            {
                return sector_at(_cursor);
            }

            bool 
            MapRenderer::cursor_is_over_control() const
            {
                Point origin_with_margin(_first.x - _DRAW_MARGIN, _first.y - _DRAW_MARGIN); 
                Point end_with_margin(_last.x + _DRAW_MARGIN, _last.y + _DRAW_MARGIN);

                return _cursor.is_between(origin_with_margin, end_with_margin);
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
        }
    }
};