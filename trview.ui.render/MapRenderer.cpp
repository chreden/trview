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
                if (_map == nullptr) return;

                const std::vector<FloorData>& data = _map->generate();
                std::for_each(data.begin(), data.end(), [&] (const FloorData& fd) {
                    draw_square(context, fd);
                });
            }

            void
            MapRenderer::draw_square(CComPtr<ID3D11DeviceContext> context, const FloorData& fd)
            {
                auto point = get_position(fd);
                auto square_colour = get_colour(fd); 
                auto texture = get_texture();
                bool ishover = false;

                if (_mouse_position.is_between(_first, _last))
                    ishover = _mouse_position.is_between(point, Point(point.x + _DRAW_SCALE - 1, point.y + _DRAW_SCALE - 1));

                _sprite.render (
                    context, texture, point.x, point.y, _DRAW_SCALE - 1, _DRAW_SCALE - 1,
                    ishover ? Color(0.7, 0.7, 0.7): square_colour
                );
            }

            Point
            MapRenderer::get_position(const FloorData& fd)
            {
                return _first + Point(fd.column * _DRAW_SCALE, fd.row * _DRAW_SCALE);
            }

            void
            MapRenderer::set_room(const trlevel::ILevel& level, const trlevel::tr3_room& room)
            {
                if (_map == nullptr)    _map = std::make_unique<Map>(level, room);
                else                    _map->load(level, room);

                _first = Point(_window_width - (_DRAW_SCALE * _map->columns()) - _DRAW_MARGIN, _DRAW_MARGIN);
                _last = _first + Point(_DRAW_SCALE * _map->columns(), _DRAW_SCALE * _map->rows());
            }

            void 
            MapRenderer::set_colours(const std::vector<FunctionColour>& colours)
            {
                _colours = colours; 
            }

            DirectX::SimpleMath::Color 
            MapRenderer::get_colour(const FloorData& fd)
            {
                if (fd.floor <= fd.ceiling)
                    return _COLOUR_WALL; 

                auto colour = std::find_if(_colours.begin(), _colours.end(), [&fd] (const FunctionColour& colour) throw() {
                    return fd.has_function(colour.function);
                }); 

                return (colour != std::end(_colours)) 
                    ? colour->colour 
                    : _COLOUR_FALLBACK;
            }

            CComPtr<ID3D11ShaderResourceView> 
            MapRenderer::get_texture()
            {
                if (_texture == nullptr) _texture = _texture_storage.coloured(0xFFFFFFFF).view;
                return _texture; 
            }

            void 
            MapRenderer::set_mouse_position(const Point& p)
            {
                _mouse_position = p;
            }
        }
    }
};