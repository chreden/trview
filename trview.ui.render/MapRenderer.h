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

            const static std::unordered_map<FunctionType, Color> 
            default_colours = {
                {FunctionType::PORTAL, Color(0.0f, 0.0f, 0.0f)},
                {FunctionType::TRIGGER, Color(1.0f, 0.3f, 0.7f)},
                {FunctionType::KILL, Color(0.8f, 0.2f, 0.2f)},
                {FunctionType::NORMAL, Color(0.0f, 0.7f, 0.7f)},
                {FunctionType::WALL, Color(0.4f, 0.4f, 0.4f)}
            };

            class MapRenderer
            {
            public:
                MapRenderer(CComPtr<ID3D11Device> device, int width, int height);

                // Renders the map 
                void render(CComPtr<ID3D11DeviceContext> context);

                // Changes the level (or room) to what is specified in the parameters 
                void load(const trlevel::ILevel& level, const trlevel::tr3_room& room);

                // Returns the sector under the specified position, or nullptr if none
                std::unique_ptr<Sector> sector_at(const Point& p) const;

                // Returns the sector that the cursor is within, or nullptr if none
                std::unique_ptr<Sector> sector_at_cursor() const;

                // Sets the colours used by the map 
                inline void set_colours(const std::unordered_map<FunctionType, Color> colours) { _colours = colours; }

                // Returns true if cursor is on the control
                bool cursor_is_over_control() const;

                // Sets the position of the cursor 
                inline void set_cursor_position(const Point& cursor) { _cursor = cursor; }

                // Returns whether the map is loaded
                inline bool loaded() const { return _map != nullptr; }

            private:
                // Generates sector positions required to render the map
                void generate_sector_positions(); 

                // Render an individual sector 
                void render_sector(CComPtr<ID3D11DeviceContext> context, const Sector& sector);

                // Gets base texture
                CComPtr<ID3D11ShaderResourceView> get_texture();

                // Determine the colour for a particular sector, based on the function(s) it has 
                Color get_colour(const Sector& sector) const; 


                CComPtr<ID3D11Device>               _device;
                int                                 _window_width, _window_height;
                std::unique_ptr<Map>                _map;
                Sprite                              _sprite; 
                TextureStorage                      _texture_storage; 
                CComPtr<ID3D11ShaderResourceView>   _texture;
                std::unordered_map<FunctionType, Color> _colours = default_colours;
                std::vector<Sector>                 _sectors; 

                Point                               _first, _last; // top-left corner, bottom-right corner (of control) 
                Point                               _cursor; // Position of the cursor 
                bool                                _is_dirty = true; // Whether we need to regenerate the map 

                const float                         _DRAW_MARGIN = 35.0f; 
                const float                         _DRAW_SCALE = 15.0f; 

                const Color                         _COLOUR_FALLBACK = Color(1.0f, 1.0f, 0.0f, 0.5f);
                const Color                         _COLOUR_WALL = Color(0.4f, 0.4f, 0.4f);
                const Color                         _COLOR_CURSOR_HIGHLIGHT = Color(0.7f, 0.7f, 0.7f);
            };
        }
    }
};
