#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <vector> 
#include <memory>
#include <algorithm>

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

            class MapRenderer
            {
            public:
                MapRenderer(CComPtr<ID3D11Device> device, int width, int height);

                // Renders the map 
                void render(CComPtr<ID3D11DeviceContext> context);

                // Changes the level (or room) to what is specified in the parameters 
                void set_room(const trlevel::ILevel& level, const trlevel::tr3_room& room);

                // Sets the colours used by the map 
                void set_colours(const std::vector<FunctionColour>& colours);

            private:
                // Draws the square at the position specified in pt using flood data in fd.  
                void draw_square(CComPtr<ID3D11DeviceContext> context, const FloorData& fd);

                // Determines where to draw a square (on the screen) based on the row/column 
                // and the screen size. Also accounts for margin, etc. 
                Point get_position(const FloorData& fd); 

                // Gets the colour to draw the square based on the floor data
                Color get_colour(const FloorData& fd);

                // Gets base texture
                CComPtr<ID3D11ShaderResourceView> get_texture(); 

                // Gets the origin point (0, 0) of where to draw the map
                Point get_origin(); 

                CComPtr<ID3D11Device>               _device;
                int                                 _window_width, _window_height;
                std::unique_ptr<Map>                _map;
                float                               _origin_x, _origin_y; 
                Sprite                              _sprite; 
                TextureStorage                      _texture_storage; 
                std::vector<FunctionColour>         _colours = default_colours; 
                CComPtr<ID3D11ShaderResourceView>   _texture;

                const float _DRAW_MARGIN = 20.0f; 
                const float _DRAW_SCALE = 15.0f; 
                const Color _COLOUR_FALLBACK = Color(0.0, 0.7, 0.7);
                const Color _COLOUR_WALL = Color(0.4, 0.4, 0.4);
            };
        }
    }
};
