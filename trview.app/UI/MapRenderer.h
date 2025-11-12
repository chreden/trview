#pragma once

#include <SimpleMath.h>

#include <vector> 
#include <memory>
#include <algorithm>

#include <trview.app/Elements/Types.h>
#include <trview.common/Point.h>
#include <trview.common/Size.h>
#include <trview.app/Elements/Room.h>
#include "Fonts/IFonts.h"
#include "../trview_imgui.h"

#include "IMapRenderer.h"
#include <trview.common/Messages/IRecipient.h>
#include <trview.common/Messages/IMessageSystem.h>


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

    class MapRenderer final : public IMapRenderer, public IRecipient, public std::enable_shared_from_this<IRecipient>
    {
    public:
        MapRenderer(const std::shared_ptr<IFonts>& fonts, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~MapRenderer() = default;
        // Renders the map 
        void render(bool window) override;
        void reset() override;
        // Changes the room to specified room, reloads map
        void load(const std::shared_ptr<trview::IRoom>& room) override;
        // Returns true if cursor is on the control
        bool cursor_is_over_control() const override;
        // Returns whether the map is loaded
        inline bool loaded() const override { return _loaded; }
        /// Set whether the map is visible.
        void set_visible(bool visible) override;
        void clear_highlight() override;
        void set_highlight(uint16_t x, uint16_t z) override;
        void set_mode(Mode mode) override;
        void set_selection(const std::shared_ptr<ISector>& sector) override;
        void set_show_tooltip(bool value) override;
        Size size() const override;
        void reposition() override;
        void receive_message(const Message& message) override;
        void set_selection(const std::vector<std::shared_ptr<ISector>>& sectors) override;
    private:
        std::shared_ptr<ISector> sector_at(const Point& p) const;
        // Determines the position (on screen) to draw a sector 
        Point get_position(const ISector& sector); 
        // Determines the size of a sector 
        Size get_size() const;
        // Draws a square at given position, size with given colour.
        void draw(ImDrawList* list, Point p, Size s, DirectX::SimpleMath::Color c);
        void tooltip();
        void clicking();

        std::weak_ptr<IMessageSystem> _messaging;
        bool _visible{ true };
        std::vector<Tile> _tiles; 
        std::uint16_t _rows, _columns; 
        bool _loaded{ false };
        bool _cursor_was_over{ false };
        std::optional<std::pair<uint16_t, uint16_t>> _highlighted_sector;
        std::shared_ptr<ISector> _previous_sector;
        Mode _mode{ Mode::Normal };
        std::optional<UserSettings> _settings;
        std::weak_ptr<ISector> _selected_sector;
        std::vector<std::weak_ptr<ISector>> _selected_sectors;
        std::shared_ptr<IFonts> _fonts;
        bool _show_tooltip{ true };
        ImGuiAnchor _anchor;

        int _reset_cycles{ 2 };
    };
};
