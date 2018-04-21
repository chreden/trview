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
                // Loaded a new level so regenerate all tiles 
                if (_is_dirty)  generate_tiles(); 

                std::for_each(_tiles.begin(), _tiles.end(), [&] (const MapTile& tile) {
                    render_tile(context, tile);
                }); 
            }

            void 
            MapRenderer::generate_tiles()
            {
                _tiles.clear(); 
                _tiles.reserve(_map->area()); 

                std::vector<Sector> sectors = _map->generate(); 

                std::for_each(sectors.begin(), sectors.end(), [&] (const Sector& sector) 
                {
                    MapTile tile; 

                    tile.origin = Point(_first + Point(sector.column * _DRAW_SCALE, sector.row * _DRAW_SCALE));
                    tile.last = Point(tile.origin.x + _DRAW_SCALE - 1, tile.origin.y + _DRAW_SCALE - 1);
                    tile.sector = sector;
                    tile.color = _COLOUR_FALLBACK;

                    // determine what colour we're going to use 
                    for (const auto& floor: tile.sector.floor_data)
                    {
                        if (_colours.find(floor.function) != _colours.end())
                            tile.color = _colours[floor.function];
                    }

                    _tiles.push_back(tile); 
                }); 
            }

            void 
            MapRenderer::render_tile(CComPtr<ID3D11DeviceContext> context, const MapTile& tile)
            {
                // Determine color, if we're over this tile then we need a specific color 
                Color color = tile.color; 
                if (_cursor.is_between(tile.origin, tile.last))
                    color.Negate(); 

                // Render tile as a sprite 
                _sprite.render(context, get_texture(), tile.origin.x, tile.origin.y, _DRAW_SCALE-1, _DRAW_SCALE-1, color);
            }

            void
            MapRenderer::load(const trlevel::ILevel& level, const trlevel::tr3_room& room)
            {
                if (_map == nullptr)    _map = std::make_unique<Map>(level, room);
                else                    _map->load(level, room);

                _first = Point(_window_width - (_DRAW_SCALE * _map->columns()) - _DRAW_MARGIN, _DRAW_MARGIN);
                _last = _first + Point(_DRAW_SCALE * _map->columns(), _DRAW_SCALE * _map->rows());
            }

            CComPtr<ID3D11ShaderResourceView>
            MapRenderer::get_texture()
            {
                if (_texture == nullptr) _texture = _texture_storage.coloured(0xFFFFFFFF).view;
                return _texture;
            }

            std::unique_ptr<MapTile> 
            MapRenderer::map_tile_at(const Point& p) const
            {
                for (const MapTile &tile: _tiles)
                {
                    if (p.is_between(tile.origin, tile.last))
                        return std::make_unique<MapTile>(tile); 
                }
            }

            bool 
            MapRenderer::cursor_is_over_control() const
            {
                Point origin_with_margin(_first.x - _DRAW_MARGIN, _first.y - _DRAW_MARGIN); 
                Point end_with_margin(_last.x + _DRAW_MARGIN, _last.y + _DRAW_MARGIN);

                return _cursor.is_between(origin_with_margin, end_with_margin);
            }

        }
    }
};