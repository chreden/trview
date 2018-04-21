#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <vector> 
#include <memory>
#include <algorithm>
#include <map>

#include "Sprite.h"
#include "trview\Map.h"
#include "trview\MapTypes.h"
#include "trview\TextureStorage.h"
#include "trview.common\Texture.h"
#include "trview.ui\Point.h"

namespace trview
{
    namespace ui
    {
        namespace render
        {
            using namespace DirectX::SimpleMath;

            const static std::unordered_map<Function, Color> 
            default_colours = {
                {Function::PORTAL, Color(0.0, 0.0, 0.0)},
                {Function::TRIGGER, Color(1.0, 0.3, 0.7)},
                {Function::KILL, Color(0.8, 0.2, 0.2)},
                {Function::NORMAL, Color(0.0, 0.7, 0.7)},
                {Function::WALL, Color(0.4, 0.4, 0.4)}
            };

            class MapRenderer
            {
            public:
                MapRenderer(CComPtr<ID3D11Device> device, int width, int height);

                // Renders the map 
                void render(CComPtr<ID3D11DeviceContext> context);

                // Changes the level (or room) to what is specified in the parameters 
                void load(const trlevel::ILevel& level, const trlevel::tr3_room& room);

                // Returns the tile under the specified position, or null if none
                std::unique_ptr<MapTile> map_tile_at(const Point& p) const;

                // Sets the colours used by the map 
                inline void set_colours(const std::unordered_map<Function, Color> colours) { _colours = colours; }

                // Returns true if cursor is on the control
                bool cursor_is_over_control() const;

                // Sets the position of the cursor 
                inline void set_cursor_position(const Point& cursor) { _cursor = cursor; }

            private:
                // Generates tiles required to render the map, each tile is a "square" in the grid 
                void generate_tiles(); 

                // Render an individual tile 
                void render_tile(CComPtr<ID3D11DeviceContext> context, const MapTile& tile);

                // Gets base texture
                CComPtr<ID3D11ShaderResourceView> get_texture();


                CComPtr<ID3D11Device>               _device;
                int                                 _window_width, _window_height;
                std::unique_ptr<Map>                _map;
                Sprite                              _sprite; 
                TextureStorage                      _texture_storage; 
                CComPtr<ID3D11ShaderResourceView>   _texture;
                std::unordered_map<Function, Color> _colours = default_colours;
                std::vector<MapTile>                _tiles; 

                Point                               _first, _last; // top-left corner, bottom-right corner
                Point                               _cursor; 
                bool                                _is_dirty = true; 

                const float                         _DRAW_MARGIN = 35.0f; 
                const float                         _DRAW_SCALE = 15.0f; 

                const Color                         _COLOUR_FALLBACK = Color(1, 1, 0, 0.5);
                const Color                         _COLOUR_WALL = Color(0.4, 0.4, 0.4);
                const Color                         _COLOR_CURSOR_HIGHLIGHT = Color(0.7, 0.7, 0.7);
            };
        }
    }
};
