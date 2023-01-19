#include "MapRenderer.h"
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/SpriteSizeStore.h>
#include <trview.graphics/ViewportStore.h>

using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

namespace trview
{
    IMapRenderer::~IMapRenderer()
    {
    }

    MapRenderer::MapRenderer(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IFontFactory>& font_factory, const Size& window_size, const graphics::ISprite::Source& sprite_source,
        const graphics::IRenderTarget::SizeSource& render_target_source)
        : _device(device),
        _window_width(static_cast<int>(window_size.width)),
        _window_height(static_cast<int>(window_size.height)),
        _sprite(sprite_source(window_size)),
        _font(font_factory->create_font("Arial", 7, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre)),
        _texture(create_texture(*device, Colour::White)),
        _render_target_source(render_target_source)
    {
        D3D11_DEPTH_STENCIL_DESC ui_depth_stencil_desc;
        memset(&ui_depth_stencil_desc, 0, sizeof(ui_depth_stencil_desc));
        _depth_stencil_state = device->create_depth_stencil_state(ui_depth_stencil_desc);
    }

    void
    MapRenderer::render()
    {
        if (!_render_target || !_visible)
        {
            return;
        }

        auto context = _device->context();
        context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);

        if (needs_redraw())
        {
            // Clear the render target to be transparent (as it may not be using the entire area).
            _render_target->clear(Color(1, 1, 1, 1));

            graphics::RenderTargetStore rs_store(context);
            graphics::ViewportStore vp_store(context);
            // Set the host size to match the render target as we will have adjusted the viewport.
            graphics::SpriteSizeStore s_store(*_sprite, _render_target->size());
 
            _render_target->apply();
            render_internal(context);
            _force_redraw = false;
        }

        if (_render_mode == RenderMode::Screen)
        {
            // Now render the render target in the correct position.
            auto p = Point(_first.x - 1, _first.y - 1);
            _sprite->render(_render_target->texture(), p.x, p.y, static_cast<float>(_render_target->width()), static_cast<float>(_render_target->height()));
        }
    }

    void
    MapRenderer::render_internal(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
    {
        // Draw base square, this is the backdrop for the map 
        draw(Point(), Size(static_cast<float>(_render_target->width()), static_cast<float>(_render_target->height())), Color(0.0f, 0.0f, 0.0f));

        std::for_each(_tiles.begin(), _tiles.end(), [&](const Tile &tile)
        {
            Color text_color = Colour::White;
            Color draw_color = _colours.colour_from_flags_field(tile.sector->flags());

            // Special case for special walls (and no space)
            if (!is_no_space(tile.sector->flags()) &&
                has_flag(tile.sector->flags(), SectorFlag::Wall) &&
                has_flag(tile.sector->flags(), SectorFlag::SpecialWall))
            {
                draw_color = _colours.colour(SectorFlag::SpecialWall);
            }

            // If the cursor is over the tile, then negate colour 
            if (tile.sector == _previous_sector ||
                 (_highlighted_sector.has_value() &&
                     _highlighted_sector.value().first == tile.sector->x() &&
                     _highlighted_sector.value().second == tile.sector->z()))
            {
                draw_color.Negate();
                text_color.Negate();
            }

            // Draw the base tile 
            draw(tile.position, tile.size, draw_color);

            // Draw climbable walls. This draws 4 separate lines - one per climbable edge. 
            // In the future I'd like to just draw a hollow square instead.
            const float thickness = _DRAW_SCALE / 4;

            if (has_flag(tile.sector->flags(), SectorFlag::Wall) && (has_flag(tile.sector->flags(), SectorFlag::Portal) || is_no_space(tile.sector->flags())))
            {
                const auto colour = _colours.colour(has_flag(tile.sector->flags(), SectorFlag::SpecialWall) ? SectorFlag::SpecialWall : SectorFlag::Wall);
                draw(tile.position, tile.size / 4.0f, colour);
            }

            if (has_flag(tile.sector->flags(), SectorFlag::ClimbableNorth))
                draw(tile.position, Size(tile.size.width, thickness), _colours.colour(SectorFlag::ClimbableNorth));
            if (has_flag(tile.sector->flags(), SectorFlag::ClimbableEast))
                draw(Point(tile.position.x + _DRAW_SCALE - thickness, tile.position.y), Size(thickness, tile.size.height), _colours.colour(SectorFlag::ClimbableEast));
            if (has_flag(tile.sector->flags(), SectorFlag::ClimbableSouth))
                draw(Point(tile.position.x, tile.position.y + _DRAW_SCALE - thickness), Size(tile.size.width, thickness), _colours.colour(SectorFlag::ClimbableSouth));
            if (has_flag(tile.sector->flags(), SectorFlag::ClimbableWest))
                draw(tile.position, Size(thickness, tile.size.height), _colours.colour(SectorFlag::ClimbableWest));

            // If sector is a down portal, draw a transparent black square over it 
            if (has_flag(tile.sector->flags(), SectorFlag::RoomBelow))
                draw(tile.position, tile.size, _colours.colour(MapColours::Special::RoomBelow));

            // If sector is an up portal, draw a small corner square in the top left to signify this 
            if (has_flag(tile.sector->flags(), SectorFlag::RoomAbove))
                draw(tile.position, Size(tile.size.width / 4, tile.size.height / 4), _colours.colour(MapColours::Special::RoomAbove));

            if (has_flag(tile.sector->flags(), SectorFlag::Death) && has_flag(tile.sector->flags(), SectorFlag::Trigger))
            {
                draw(tile.position + Point(tile.size.width * 0.75f, 0), tile.size / 4.0f, _colours.colour(SectorFlag::Death));
            }

            if (has_flag(tile.sector->flags(), SectorFlag::Portal))
            {
                _font->render(context, std::to_wstring(tile.sector->portal()), tile.position.x - 1, tile.position.y, tile.size.width, tile.size.height, text_color);
            }

            if (_selected_sector.lock() == tile.sector)
            {
                draw(tile.position, { tile.size.width, 1.0f }, Colour::Yellow);
                draw(tile.position + Point(0, 1.0f), { 1.0f, tile.size.height - 2.0f }, Colour::Yellow);
                draw(tile.position + Point(0, tile.size.height - 1.0f), { tile.size.width, 1.0f }, Colour::Yellow);
                draw(tile.position + Point(tile.size.width - 1.0f, 1.0f), { 1.0f, tile.size.height - 2.0f }, Colour::Yellow);
            }
        });
    }

    void 
    MapRenderer::draw(Point p, Size s, Color c)
    {
        _sprite->render(_texture, p.x, p.y, s.width, s.height, c); 
    }

    void
    MapRenderer::load(const std::shared_ptr<trview::IRoom>& room)
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
        std::for_each(sectors.begin(), sectors.end(), 
            [&] (const auto& sector) 
        {
            _tiles.emplace_back(sector, get_position(*sector), get_size());
        });

        _previous_sector.reset();
        on_sector_hover(nullptr);
    }

    Point MapRenderer::get_position(const ISector& sector)
    {
        return Point {
            /* X */ _DRAW_SCALE * sector.x(),
            /* Y */ _rows * _DRAW_SCALE - _DRAW_SCALE * (sector.z() + 1)
        } + Point(1,1); 
    }

    Size MapRenderer::get_size() const
    {
        return Size { _DRAW_SCALE - 1, _DRAW_SCALE - 1 };
    }

    std::shared_ptr<ISector> 
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
            return iter->sector;
    }

    std::shared_ptr<ISector> 
    MapRenderer::sector_at_cursor() const
    {
        if (!_visible)
        {
            return nullptr;
        }
        return sector_at(_cursor);
    }

    bool 
    MapRenderer::cursor_is_over_control() const
    {
        return _render_target && _visible && _cursor.is_between(Point(), Point(static_cast<float>(_render_target->width()), static_cast<float>(_render_target->height())));
    }

    void MapRenderer::set_cursor_position(const Point& cursor)
    {
        _cursor = cursor - _first;

        auto sector = sector_at_cursor();
        if(sector != _previous_sector)
        {
            _previous_sector = sector;
            on_sector_hover(sector);
        }
    }

    void 
    MapRenderer::set_window_size(const Size& size)
    {
        _window_width = static_cast<int>(size.width);
        _window_height = static_cast<int>(size.height);
        _sprite->set_host_size(size);
        update_map_position();
    }

    Point MapRenderer::first() const
    {
        return _first;
    }

    void 
    MapRenderer::update_map_position()
    {
        const float margin = _render_mode == RenderMode::Screen ? _DRAW_MARGIN : 0;
        // Location of the origin of the control 
        _first = Point(_window_width - (_DRAW_SCALE * _columns) - margin, margin);
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
            _render_target = _render_target_source(width, height, graphics::IRenderTarget::DepthStencilMode::Disabled);
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

    void MapRenderer::set_visible(bool visible)
    {
        _visible = visible;
    }

    void MapRenderer::clear_highlight()
    {
        if (_highlighted_sector.has_value())
        {
            _highlighted_sector.reset();
            _force_redraw = true;
        }
    }

    void MapRenderer::set_highlight(uint16_t x, uint16_t z)
    {
        if (x > _columns || z > _rows ||
            (_highlighted_sector.has_value() && x == _highlighted_sector.value().first && z == _highlighted_sector.value().second))
        {
            return;
        }

        _highlighted_sector = { x, z };
        _force_redraw = true;
    }

    graphics::Texture MapRenderer::texture() const
    {
        return _render_target->texture();
    }

    void MapRenderer::set_render_mode(RenderMode mode)
    {
        _render_mode = mode;
        update_map_position();
    }

    void MapRenderer::set_colours(const MapColours& colours)
    {
        _colours = colours;
        _force_redraw = true;
    }

    void MapRenderer::set_selection(const std::shared_ptr<ISector>& sector)
    {
        _selected_sector = sector;
        _force_redraw = true;
    }

    void MapRenderer::clear_hovered_sector()
    {
        _previous_sector.reset();
        _force_redraw = true;
    }
};