#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

#include <vector> 
#include <memory>
#include <algorithm>
#include <map>

#include <trview.graphics/Sprite.h>
#include <trview.app/Elements/Types.h>
#include <trview.graphics/Texture.h>
#include <trview.common/Point.h>
#include <trview.common/Size.h>
#include <trview.app/Elements/Room.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/IFont.h>

#include "IMapRenderer.h"

namespace trview
{
    namespace
    {
        struct Tile
        {
        public:
            Tile(const std::shared_ptr<ISector>& p_sector, Point p_position, Size p_size)
                : sector(p_sector), position(p_position), size(p_size) {}

            std::shared_ptr<ISector> sector; 
            Point position; 
            Size size; 
        };
    }

    class MapRenderer final : public IMapRenderer
    {
    public:
        MapRenderer(const std::shared_ptr<graphics::IDevice>& device,
            const graphics::IFontFactory& font_factory,
            const Size& window_size,
            const graphics::ISprite::Source& sprite_source,
            const graphics::IRenderTarget::SizeSource& render_target_source);

        virtual ~MapRenderer() = default;

        // Renders the map 
        virtual void render(bool to_screen = true) override;

        // Changes the room to specified room, reloads map
        virtual void load(const std::shared_ptr<trview::IRoom>& room) override;

        // Returns the total area of the room 
        virtual inline std::uint16_t area() const override { return _columns * _rows; }

        // Returns the sector under the specified position, or nullptr if none
        virtual std::shared_ptr<ISector> sector_at(const Point& p) const override;

        // Returns the sector that the cursor is within, or nullptr if none
        virtual std::shared_ptr<ISector> sector_at_cursor() const override;

        // Returns true if cursor is on the control
        virtual bool cursor_is_over_control() const override;

        // Sets the position of the cursor 
        virtual void set_cursor_position(const Point& cursor) override;

        // Returns whether the map is loaded
        virtual inline bool loaded() const override { return _loaded; }

        // Set the size of the host window.
        virtual void set_window_size(const Size& size) override;

        /// Set whether the map is visible.
        virtual void set_visible(bool visible) override;

        virtual void clear_highlight() override;

        virtual void set_highlight(uint16_t x, uint16_t z) override;

        virtual graphics::Texture texture() const override;

        virtual Point first() const override;
    private:
        // Determines the position (on screen) to draw a sector 
        Point get_position(const ISector& sector); 

        // Determines the size of a sector 
        Size get_size() const;

        // Draws a square at given position, size with given colour.
        void draw(Point p, Size s, DirectX::SimpleMath::Color c);

        // Update the stored positions of the corners of the map.
        void update_map_position();

        // Update the render target that the squares will be rendered to, depending
        // on the size of the room (based on columns and rows).
        void update_map_render_target();

        // Render the map squares and the background.
        void render_internal(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

        // Determines if the minimap needs to be re-drawn.
        bool needs_redraw();

        std::shared_ptr<graphics::IDevice>                 _device;
        bool                                               _visible = true;
        int                                                _window_width, _window_height;
        std::unique_ptr<graphics::ISprite>                 _sprite; 
        graphics::Texture                                  _texture;
        std::vector<Tile>                                  _tiles; 
        std::unique_ptr<graphics::IRenderTarget>           _render_target;
        graphics::IRenderTarget::SizeSource                _render_target_source;

        Point                               _first, _last; // top-left corner, bottom-right corner (of control) 
        Point                               _cursor; // Position of the cursor relative to the top left of the control.
        std::uint16_t                       _rows, _columns; 
        bool                                _loaded = false;
                
        bool                                _cursor_was_over = false;
        bool                                _force_redraw = true;

        const float                         _DRAW_MARGIN = 30.0f; 
        const float                         _DRAW_SCALE = 17.0f; 

        std::optional<std::pair<uint16_t, uint16_t>> _selected_sector;
        std::unique_ptr<graphics::IFont> _font;
        std::shared_ptr<ISector> _previous_sector;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
    };
};
