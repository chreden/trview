#include "RouteWindow.h"
#include <trview.app/Routing/Route.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Button.h>
#include <trview.common/Strings.h>

namespace trview
{
    using namespace ui;

    namespace
    {
        std::wstring pos_to_string(const DirectX::SimpleMath::Vector3& position)
        {
            return std::to_wstring(static_cast<int>(position.x * 1024)) + L", " +
                std::to_wstring(static_cast<int>(position.y * 1024)) + L", " + 
                std::to_wstring(static_cast<int>(position.z * 1024));
        }

        Listbox::Item make_item(const std::wstring& name, const std::wstring& value)
        {
            return Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };
    }

    namespace Colours
    {
        const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
        const Colour LeftPanel{ 1.0f, 0.25f, 0.25f, 0.25f };
        const Colour ItemDetails{ 1.0f, 0.225f, 0.225f, 0.225f };
        const Colour Notes{ 1.0f, 0.20f, 0.20f, 0.20f };
        const Colour NotesTextArea{ 1.0f, 0.15f, 0.15f, 0.15f };
        const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
    }

    using namespace graphics;

    RouteWindow::RouteWindow(Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, const trview::Window& parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.route", L"Route", Size(470, 400))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    void RouteWindow::set_route(Route* route) 
    {
        _route = route;
        _selected_index = 0u;

        std::vector<Listbox::Item> items;
        for (uint32_t i = 0; i < _route->waypoints(); ++i)
        {
            items.push_back(create_listbox_item(i, _route->waypoint(i)));
        }
        _waypoints->set_items(items);
        load_waypoint_details(_selected_index);

        const auto colour = route->colour();
        _colour->set_text_colour(colour);
        _colour->set_text_background_colour(colour);
        _colour->set_selected_value(colour.name());
    }

    std::unique_ptr<Control> RouteWindow::create_left_panel()
    {
        auto left_panel = std::make_unique<StackPanel>(Size(200, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        auto buttons = std::make_unique<StackPanel>(Size(200, 20), Colours::LeftPanel, Size(0, 0), StackPanel::Direction::Horizontal);

        _colour = buttons->add_child(std::make_unique<Dropdown>(Size(20, 20)));
        _colour->set_text_colour(Colour::Green);
        _colour->set_text_background_colour(Colour::Green);
        _colour->set_values(
            {
                Dropdown::Value { Colour::Green.name(), Colour::Green, Colour::Green },
                { Colour::Red.name(), Colour::Red, Colour::Red },
                { Colour::Blue.name(), Colour::Blue, Colour::Blue },
                { Colour::Yellow.name(), Colour::Yellow, Colour::Yellow },
                { Colour::Cyan.name(), Colour::Cyan, Colour::Cyan },
                { Colour::Magenta.name(), Colour::Magenta, Colour::Magenta },
                { Colour::Black.name(), Colour::Black, Colour::Black },
                { Colour::White.name(), Colour::White, Colour::White }
            });
        _colour->set_selected_value(Colour::Green.name());
        _colour->set_dropdown_scope(_ui.get());

        _token_store += _colour->on_value_selected += [=](const auto& value)
        {
            const auto new_colour = named_colour(value);
            _colour->set_text_colour(new_colour);
            _colour->set_text_background_colour(new_colour);
            on_colour_changed(new_colour);
        };

        auto import = buttons->add_child(std::make_unique<Button>(Size(90, 20), L"Import"));
        _token_store += import->on_click += [&]()
        {
            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));

            wchar_t path[MAX_PATH];
            memset(&path, 0, sizeof(path));

            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFilter = L"trview route\0*.tvr\0";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrTitle = L"Import route";
            ofn.Flags = OFN_FILEMUSTEXIST;
            ofn.lpstrFile = path;
            if (GetOpenFileName(&ofn))
            {
                on_route_import(trview::to_utf8(ofn.lpstrFile));
            }
        };
        auto export_button = buttons->add_child(std::make_unique<Button>(Size(90, 20), L"Export"));
        _token_store += export_button->on_click += [&]()
        {
            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));

            wchar_t path[MAX_PATH];
            memset(&path, 0, sizeof(path));

            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFilter = L"trview route\0*.tvr\0";
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrTitle = L"Export route";
            ofn.lpstrFile = path;
            ofn.lpstrDefExt = L"tvr";
            if (GetSaveFileName(&ofn))
            {
                on_route_export(trview::to_utf8(ofn.lpstrFile));
            }
        };
        auto _buttons = left_panel->add_child(std::move(buttons));

        // List box to show the waypoints in the route.
        auto waypoints = std::make_unique<Listbox>(Size(200, window().size().height - _buttons->size().height), Colours::LeftPanel);
        waypoints->set_enable_sorting(false);
        waypoints->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::String, L"Type", 160 }
            }
        );
        _token_store += waypoints->on_item_selected += [&](const auto& item) {
            auto index = std::stoi(item.value(L"#"));
            load_waypoint_details(index);
            on_waypoint_selected(index);
        };
        _token_store += waypoints->on_delete += [&]() {
            on_waypoint_deleted(_selected_index);
        };

        _waypoints = left_panel->add_child(std::move(waypoints));
        return left_panel;
    }

    std::unique_ptr<Control> RouteWindow::create_right_panel()
    {
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        right_panel->set_margin(Size(0, 8));

        auto group_box = std::make_unique<GroupBox>(Size(panel_width, 140), Colours::ItemDetails, Colours::DetailsBorder, L"Waypoint Details");

        auto details_panel = std::make_unique<StackPanel>(Point(10, 21), Size(panel_width - 20, 120), Colours::ItemDetails, Size(0, 8), StackPanel::Direction::Vertical, SizeMode::Manual);

        auto stats_box = std::make_unique<Listbox>(Point(10, 21), Size(panel_width - 20, 80), Colours::ItemDetails);
        stats_box->set_show_headers(false);
        stats_box->set_show_scrollbar(false);
        stats_box->set_columns(
            {
                { Listbox::Column::Type::String, L"Name", 100 },
                { Listbox::Column::Type::String, L"Value", 150 }
            });
        _token_store += stats_box->on_item_selected += [&](const auto&)
        {
            const auto index = _route->waypoint(_selected_index).index();
            switch (_selected_type)
            {
            case Waypoint::Type::Entity:
                if (index < _all_items.size())
                {
                    on_item_selected(_all_items[index]);
                }
                break;
            case Waypoint::Type::Trigger:
                if (index < _all_triggers.size())
                {
                    on_trigger_selected(_all_triggers[index]);
                }
                break;
            }
        };
        _stats = details_panel->add_child(std::move(stats_box));

        auto delete_button = details_panel->add_child(std::make_unique<Button>(Size(panel_width - 20, 20), L"Delete Waypoint"));
        _token_store += delete_button->on_click += [&]()
        {
            if (_route && _selected_index < _route->waypoints())
            {
                on_waypoint_deleted(_selected_index);
            }
        };

        group_box->add_child(std::move(details_panel));
        right_panel->add_child(std::move(group_box));

        // Notes area.
        auto notes_box = std::make_unique<GroupBox>(Size(panel_width, window().size().height - 140), Colours::Notes, Colours::DetailsBorder, L"Notes");

        auto notes_area = std::make_unique<TextArea>(Point(10, 21), Size(panel_width - 20, notes_box->size().height - 41), Colours::NotesTextArea, Colour(1.0f, 1.0f, 1.0f));
        _notes_area = notes_box->add_child(std::move(notes_area));

        right_panel->add_child(std::make_unique<ui::Window>(Size(panel_width, 5), Colours::Notes));
        right_panel->add_child(std::move(notes_box));

        _token_store += _notes_area->on_text_changed += [&](const std::wstring& text)
        {
            if (_route && _selected_index < _route->waypoints())
            {
                _route->waypoint(_selected_index).set_notes(text);
            }
        };

        return right_panel;
    }

    Listbox::Item RouteWindow::create_listbox_item(uint32_t index, const Waypoint& waypoint)
    {
        std::wstring type = waypoint_type_to_string(waypoint.type());
        if (waypoint.type() == Waypoint::Type::Entity)
        {
            if (waypoint.index() < _all_items.size())
            {
                type = _all_items[waypoint.index()].type();
            }
            else
            {
                type = L"Invalid entity";
            }
        }
        else if (waypoint.type() == Waypoint::Type::Trigger)
        {
            if (waypoint.index() < _all_triggers.size())
            {
                type = trigger_type_name(_all_triggers[waypoint.index()]->type());
            }
            else
            {
                type = L"Invalid trigger";
            }
        }
        return { {{ L"#", std::to_wstring(index) },
                 { L"Type", type}} };
    }

    void RouteWindow::load_waypoint_details(uint32_t index)
    {
        if (index >= _route->waypoints())
        {
            _stats->set_items({});
            _notes_area->set_text(L"");
            return;
        }

        const auto& waypoint = _route->waypoint(index);
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", waypoint_type_to_string(waypoint.type())));
        stats.push_back(make_item(L"Position", pos_to_string(waypoint.position())));

        _selected_type = waypoint.type();
        _selected_index = index;

        if (waypoint.type() != Waypoint::Type::Position)
        {
            stats.push_back(make_item(L"Target Index", std::to_wstring(waypoint.index())));
            if (waypoint.type() == Waypoint::Type::Entity)
            {
                std::wstring type = L"Invalid entity";
                if (waypoint.index() < _all_items.size())
                {
                    type = _all_items[waypoint.index()].type();
                }
                stats.push_back(make_item(L"Entity", type));
            }
            else if (waypoint.type() == Waypoint::Type::Trigger)
            {
                std::wstring type = L"Invalid trigger";
                if (waypoint.index() < _all_items.size())
                {
                    type = trigger_type_name(_all_triggers[waypoint.index()]->type());
                }
                stats.push_back(make_item(L"Trigger Type", type));
            }
        }

        _stats->set_items(stats);

        _notes_area->set_text(waypoint.notes());
    }

    void RouteWindow::select_waypoint(uint32_t index)
    {
        _selected_index = index;

        if (!_route)
        {
            return;
        }

        if (index < _route->waypoints())
        {
            _waypoints->set_selected_item(create_listbox_item(index, _route->waypoint(index)));
            load_waypoint_details(index);
        }
    }

    /// Set the items to that are in the level.
    /// @param items The items to show.
    void RouteWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }

    /// Set the triggers in the level.
    /// @param triggers The triggers.
    void RouteWindow::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _all_triggers = triggers;
    }
}
