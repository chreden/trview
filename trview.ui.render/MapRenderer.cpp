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
                // No map loaded so nothing to render 
                if (!_map)      return; 
                // Loaded a new level so regenerate position of sectors
                if (_is_dirty)
                {
                    // Regenerate map sectors
                    _map->generate();
                    // Generate positions for each sector and store them alongside the sector
                    generate_sector_positions();
                }

                // Iterate over each sectors and render each one individually
                std::for_each(_sectors.begin(), _sectors.end(), [&] (const Sector& sector) {
                    render_sector(context, sector);
                }); 
            }

            void 
            MapRenderer::generate_sector_positions()
            {
                // Generate map sectors 
                // This method will do no work if no new level/room has been loaded 
                _sectors = _map->generate(); 

                // Nothing to do 
                if (_sectors.empty()) return; 

                // Loop over all sectors and determine where they should be drawn 
                std::for_each(_sectors.begin(), _sectors.end(), [&] (Sector& sector) 
                {
                    sector.origin = Point(_first + Point(sector.column * _DRAW_SCALE, sector.row * _DRAW_SCALE));
                    sector.last = Point(sector.origin.x + _DRAW_SCALE - 1, sector.origin.y + _DRAW_SCALE - 1);
                }); 
            }

            void 
            MapRenderer::render_sector(CComPtr<ID3D11DeviceContext> context, const Sector& sector)
            {
                // Determine color, if we're over this tile then we negate the colour
                Color color = get_colour(sector); 
                if (_cursor.is_between(sector.origin, sector.last))
                    color.Negate(); 

                // Render tile as a sprite 
                _sprite.render(context, get_texture(), sector.origin.x, sector.origin.y, _DRAW_SCALE-1, _DRAW_SCALE-1, color);
            }

            Color 
            MapRenderer::get_colour(const Sector& sector) const
            {
                // Rank sector function types - currently we are limited to showing one colour at once
                // Portal is the primary preference 
                if (sector.is_portal)
                    return _colours.at(FunctionType::PORTAL);
                else if (sector.is_wall())
                    return _colours.at(FunctionType::WALL);
                else if (sector.is_trigger)
                    return _colours.at(FunctionType::TRIGGER);
                else if (sector.is_death)
                    return _colours.at(FunctionType::KILL);
                else
                    return _colours.at(FunctionType::NORMAL);
            }

            void
            MapRenderer::load(const trlevel::ILevel& level, const trlevel::tr3_room& room)
            {
                // If this is the first time calling load, we need to create Map class 
                if (_map == nullptr)    _map = std::make_unique<Map>(level, room);
                // Otherwise just call load() 
                else                    _map->load(level, room);

                update_map_position();
            }

            CComPtr<ID3D11ShaderResourceView>
            MapRenderer::get_texture()
            {
                if (_texture == nullptr) _texture = _texture_storage.coloured(0xFFFFFFFF).view;
                return _texture;
            }

            std::unique_ptr<Sector> 
            MapRenderer::sector_at(const Point& p) const
            {
                for (const Sector &sector: _sectors)
                {
                    if (p.is_between(sector.origin, sector.last))
                        return std::make_unique<Sector>(sector);
                }
            }

            std::unique_ptr<Sector> 
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
                _first = Point(_window_width - (_DRAW_SCALE * _map->columns()) - _DRAW_MARGIN, _DRAW_MARGIN);
                // Location of the last point of the control (bottom-right)
                _last = _first + Point(_DRAW_SCALE * _map->columns(), _DRAW_SCALE * _map->rows());
            }
        }
    }
};