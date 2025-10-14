#include "MapRenderer.h"
#include "../Elements/ILevel.h"
#include <ranges>
#include <trview.common/Messages/Message.h>

using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

namespace trview
{
    namespace
    {
        constexpr float _DRAW_MARGIN = 30.0f;
        constexpr float _DRAW_SCALE = 17.0f;
    }

    IMapRenderer::~IMapRenderer()
    {
    }

    MapRenderer::MapRenderer(const std::shared_ptr<IFonts>& fonts, const std::weak_ptr<IMessageSystem>& messaging)
        : _fonts(fonts), _messaging(messaging)
    {
    }

    void MapRenderer::render(bool window)
    {
        if (!_settings)
        {
            if (auto messaging = _messaging.lock())
            {
                messaging->send_message(Message{ .type = "get_settings", .data = std::make_shared<MessageData<std::weak_ptr<IRecipient>>>(weak_from_this()) });
            }
            return;
        }

        if (!_visible || !_loaded)
        {
            return;
        }

        const float width = static_cast<float>(_DRAW_SCALE * _columns + 1);
        const float height = static_cast<float>(_DRAW_SCALE * _rows + 1);

        bool is_reset = _reset_cycles > 0;
        if (window)
        {
            if (is_reset)
            {
                --_reset_cycles;
            }

            const auto vp = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(vp->Pos + ImVec2(vp->Size.x, 0) + ImVec2(-20, 20), 
                is_reset ? ImGuiCond_Always : ImGuiCond_Appearing, ImVec2(1, 0));

            if (!is_reset)
            {
                _anchor.check_resize({ width, height });
            }

            if (!ImGui::Begin("Minimap", nullptr, ImGuiWindowFlags_NoResize))
            {
                ImGui::End();
                return;
            }
        }

        if (!is_reset)
        {
            _anchor.record_position({ width, height });
        }
        auto list = ImGui::GetWindowDrawList();
        const auto cursorPos = ImGui::GetCursorPos();
        const auto pos = ImGui::GetWindowPos() + cursorPos;

        // Cursor hovering:
        _cursor_was_over = ImGui::IsMouseHoveringRect(pos, pos + ImVec2(width, height));
        const auto mouse_pos = ImGui::GetMousePos() - pos;
        const auto sector = sector_at({ mouse_pos.x, mouse_pos.y });
        if (sector != _previous_sector)
        {
            _previous_sector = sector;
            on_sector_hover(sector);
        }
        tooltip();

        ImGui::PushFont(_fonts->font("Minimap"));

        // Background rectangle.
        draw(list, Point(), Size(width, height), Color(0.0f, 0.0f, 0.0f));

        const auto settings = _settings.value();
        std::for_each(_tiles.begin(), _tiles.end(), [&](const Tile& tile)
            {
                const ImVec2 tile_pos = ImVec2(tile.position.x, tile.position.y);
                const ImVec2 tile_size = ImVec2(tile.size.width, tile.size.height);

                Color text_color = Colour::White;
                Color draw_color = settings.map_colours.colour_from_flags_field(tile.sector->flags());

                // Special case for special walls (and no space)
                if (!is_no_space(tile.sector->flags()) &&
                    has_flag(tile.sector->flags(), SectorFlag::Wall) &&
                    has_flag(tile.sector->flags(), SectorFlag::SpecialWall))
                {
                    draw_color = settings.map_colours.colour(SectorFlag::SpecialWall);
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
                draw(list, tile.position, tile.size, draw_color);

                // Draw climbable walls. This draws 4 separate lines - one per climbable edge. 
                // In the future I'd like to just draw a hollow square instead.
                const float thickness = _DRAW_SCALE / 4;

                if (has_flag(tile.sector->flags(), SectorFlag::Wall) && (has_flag(tile.sector->flags(), SectorFlag::Portal) || is_no_space(tile.sector->flags())))
                {
                    const auto colour = settings.map_colours.colour(has_flag(tile.sector->flags(), SectorFlag::SpecialWall) ? SectorFlag::SpecialWall : SectorFlag::Wall);
                    draw(list, tile.position, tile.size / 4.0f, colour);
                }

                if (has_flag(tile.sector->flags(), SectorFlag::ClimbableNorth))
                    draw(list, tile.position, Size(tile.size.width, thickness), settings.map_colours.colour(SectorFlag::ClimbableNorth));
                if (has_flag(tile.sector->flags(), SectorFlag::ClimbableEast))
                    draw(list, Point(tile.position.x + _DRAW_SCALE - thickness, tile.position.y), Size(thickness, tile.size.height), settings.map_colours.colour(SectorFlag::ClimbableEast));
                if (has_flag(tile.sector->flags(), SectorFlag::ClimbableSouth))
                    draw(list, Point(tile.position.x, tile.position.y + _DRAW_SCALE - thickness), Size(tile.size.width, thickness), settings.map_colours.colour(SectorFlag::ClimbableSouth));
                if (has_flag(tile.sector->flags(), SectorFlag::ClimbableWest))
                    draw(list, tile.position, Size(thickness, tile.size.height), settings.map_colours.colour(SectorFlag::ClimbableWest));

                // If sector is a down portal, draw a transparent black square over it 
                if (has_flag(tile.sector->flags(), SectorFlag::RoomBelow))
                    draw(list, tile.position, tile.size, settings.map_colours.colour(MapColours::Special::RoomBelow));

                // If sector is an up portal, draw a small corner square in the top left to signify this 
                if (has_flag(tile.sector->flags(), SectorFlag::RoomAbove))
                    draw(list, tile.position, Size(tile.size.width / 4, tile.size.height / 4), settings.map_colours.colour(MapColours::Special::RoomAbove));

                if (has_flag(tile.sector->flags(), SectorFlag::Death) && has_flag(tile.sector->flags(), SectorFlag::Trigger))
                {
                    draw(list, tile.position + Point(tile.size.width * 0.75f, 0), tile.size / 4.0f, settings.map_colours.colour(SectorFlag::Death));
                }

                if (has_flag(tile.sector->flags(), SectorFlag::Portal))
                {
                    const std::string text = std::format("{}{}", tile.sector->portals()[0], tile.sector->portals().size() > 1 ? "+" : "");
                    auto text_size = ImGui::CalcTextSize(text.c_str());
                    list->AddText(
                        pos + 
                        ImVec2(tile.position.x, tile.position.y) + 
                        (ImVec2(tile.size.width, tile.size.height) - text_size) * 0.5f, ImColor(text_color.R(), text_color.G(), text_color.B()),
                        text.c_str());
                }

                if (_selected_sector.lock() == tile.sector)
                {
                    draw(list, tile.position, { tile.size.width, 1.0f }, Colour::Yellow);
                    draw(list, tile.position + Point(0, 1.0f), { 1.0f, tile.size.height - 2.0f }, Colour::Yellow);
                    draw(list, tile.position + Point(0, tile.size.height - 1.0f), { tile.size.width, 1.0f }, Colour::Yellow);
                    draw(list, tile.position + Point(tile.size.width - 1.0f, 1.0f), { 1.0f, tile.size.height - 2.0f }, Colour::Yellow);
                }
            });
        ImGui::PopFont();
        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(width, height));

        clicking();

        if (window)
        {
            _anchor.record_size();
            ImGui::End();
        }
    }

    void MapRenderer::reset()
    {
        _reset_cycles = 2;
    }

    void MapRenderer::draw(ImDrawList* list, Point p, Size s, DirectX::SimpleMath::Color c)
    {
        const auto pos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
        list->AddRectFilled(
            pos + ImVec2(p.x, p.y),
            pos + ImVec2(p.x, p.y) + ImVec2(s.width, s.height),
            ImColor(c.R(), c.G(), c.B(), c.A()));
    }

    void MapRenderer::load(const std::shared_ptr<trview::IRoom>& room)
    {
        _tiles.clear();
        _previous_sector.reset();
        on_sector_hover({});

        if (!room)
        {
            _loaded = false;
            return;
        }

        // Set window position and size 
        _columns = room->num_x_sectors();
        _rows = room->num_z_sectors();
        _loaded = true; 

        _tiles = room->sectors() 
            | std::views::transform([&](auto&& s) { return Tile(s, get_position(*s), get_size()); })
            | std::ranges::to<std::vector>();
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

    std::shared_ptr<ISector> MapRenderer::sector_at(const Point& p) const
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

    bool MapRenderer::cursor_is_over_control() const
    {
        return _cursor_was_over;
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
    }

    void MapRenderer::set_mode(Mode mode)
    {
        _mode = mode;
    }

    void MapRenderer::set_selection(const std::shared_ptr<ISector>& sector)
    {
        _selected_sector = sector;
    }

    void MapRenderer::tooltip()
    {
        if (_show_tooltip && _previous_sector)
        {
            std::string text = std::format("X: {}, Z: {}\n", _previous_sector->x(), _previous_sector->z());
            if (has_flag(_previous_sector->flags(), SectorFlag::RoomAbove))
            {
                text += std::format("Above: {}", _previous_sector->room_above());
            }
            if (has_flag(_previous_sector->flags(), SectorFlag::RoomBelow))
            {
                text += std::format("{}Below: {}", has_flag(_previous_sector->flags(), SectorFlag::RoomAbove) ? ", " : "", _previous_sector->room_below());
            }
            ImGui::SetTooltip(text.c_str());
        }
    }

    void MapRenderer::set_show_tooltip(bool value)
    {
        _show_tooltip = value;
    }

    void MapRenderer::clicking()
    {
        if (!_settings)
        {
            return;
        }

        const auto settings = _settings.value();
        if (_previous_sector)
        {
            if (const auto room = _previous_sector->room().lock())
            {
                if (const auto level = room->level().lock())
                {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        if (_mode == Mode::Select)
                        {
                            _selected_sector = _previous_sector;
                            on_sector_selected(_selected_sector);
                            return;
                        }

                        auto trigger = _previous_sector->trigger().lock();
                        if (!trigger || (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)))
                        {
                            if (has_flag(_previous_sector->flags(), SectorFlag::Portal))
                            {
                                on_room_selected(level->room(_previous_sector->portals()[0]));
                            }
                            else if (!settings.invert_map_controls && has_flag(_previous_sector->flags(), SectorFlag::RoomBelow))
                            {
                                on_room_selected(level->room(_previous_sector->room_below()));
                            }
                            else if (settings.invert_map_controls && has_flag(_previous_sector->flags(), SectorFlag::RoomAbove))
                            {
                                on_room_selected(level->room(_previous_sector->room_above()));
                            }
                        }
                        else
                        {
                            on_trigger_selected(trigger);
                        }
                    }
                    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && _mode == Mode::Normal)
                    {
                        if (!settings.invert_map_controls && has_flag(_previous_sector->flags(), SectorFlag::RoomAbove))
                        {
                            on_room_selected(level->room(_previous_sector->room_above()));
                        }
                        else if (settings.invert_map_controls && has_flag(_previous_sector->flags(), SectorFlag::RoomBelow))
                        {
                            on_room_selected(level->room(_previous_sector->room_below()));
                        }
                    }
                }
            }
        }
    }

    Size MapRenderer::size() const
    {
        const float width = static_cast<float>(_DRAW_SCALE * _columns + 1);
        const float height = static_cast<float>(_DRAW_SCALE * _rows + 1);
        return Size(width, height);
    }

    void MapRenderer::reposition()
    {
        _anchor.reposition = true;
    }

    void MapRenderer::receive_message(const Message& message)
    {
        if (message.type == "settings")
        {
            _settings = std::static_pointer_cast<MessageData<UserSettings>>(message.data)->value;
        }
    }
}
