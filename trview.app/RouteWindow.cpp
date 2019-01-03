#include "RouteWindow.h"
#include "Waypoint.h"
#include "Route.h"
#include <trview.ui/GroupBox.h>
#include <trview.ui/TextArea.h>

namespace trview
{
    using namespace ui;

    namespace
    {
        std::wstring waypoint_type_to_string(Waypoint::Type type)
        {
            switch (type)
            {
            case Waypoint::Type::Entity:
                return L"Entity";
            case Waypoint::Type::Position:
                return L"Position";
            case Waypoint::Type::Trigger:
                return L"Trigger";
            }
            return L"Unknown";
        }

        std::wstring pos_to_string(const DirectX::SimpleMath::Vector3& position)
        {
            return std::to_wstring(static_cast<int>(position.x * 1024)) + L", " +
                std::to_wstring(static_cast<int>(position.y * 1024)) + L", " + 
                std::to_wstring(static_cast<int>(position.z * 1024));
        }

        Listbox::Item create_listbox_item(uint32_t index, const Waypoint& waypoint)
        {
            return { {{ L"#", std::to_wstring(index) },
                     { L"Type", waypoint_type_to_string(waypoint.type())}} };
        }

        Listbox::Item make_item(const std::wstring& name, const std::wstring& value)
        {
            return Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };
    }

    namespace Colours
    {
        const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
        const Colour LeftPanel{ 1.0f, 0.4f, 0.4f, 0.4f };
        const Colour ItemDetails{ 1.0f, 0.35f, 0.35f, 0.35f };
        const Colour Notes{ 1.0f, 0.3f, 0.3f, 0.3f };
        const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
    }

    using namespace graphics;

    RouteWindow::RouteWindow(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.route", L"Route", Size(470, 400)), _keyboard(window())
    {
        set_panels(create_left_panel(), create_right_panel());

        _token_store.add(_keyboard.on_char += [&](auto character)
        {
            _notes_area->handle_char(character);
        });
    }

    void RouteWindow::load_waypoints(const Route& route) 
    {
        _all_waypoints.clear();

        std::vector<Listbox::Item> items;
        for (uint32_t i = 0; i < route.waypoints(); ++i)
        {
            items.push_back(create_listbox_item(i, route.waypoint(i)));
            _all_waypoints.push_back(route.waypoint(i));
        }
        _waypoints->set_items(items);
    }

    std::unique_ptr<Control> RouteWindow::create_left_panel()
    {
        auto left_panel = std::make_unique<StackPanel>(Point(), Size(200, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        // List box to show the waypoints in the route.
        auto waypoints = std::make_unique<Listbox>(Point(), Size(200, window().size().height), Colours::LeftPanel);
        waypoints->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::String, L"Type", 160 }
            }
        );
        _token_store.add(waypoints->on_item_selected += [&](const auto& item) {
            auto index = std::stoi(item.value(L"#"));
            load_waypoint_details(index);
            on_waypoint_selected(index);
        });
        _waypoints = left_panel->add_child(std::move(waypoints));
        return left_panel;
    }

    std::unique_ptr<Control> RouteWindow::create_right_panel()
    {
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Point(), Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        right_panel->set_margin(Size(0, 8));

        auto group_box = std::make_unique<GroupBox>(Point(), Size(panel_width, 110), Colours::ItemDetails, Colours::DetailsBorder, L"Waypoint Details");

        auto stats_box = std::make_unique<Listbox>(Point(10, 21), Size(panel_width - 20, 100), Colours::ItemDetails);
        stats_box->set_show_headers(false);
        stats_box->set_show_scrollbar(false);
        stats_box->set_columns(
            {
                { Listbox::Column::Type::String, L"Name", 100 },
                { Listbox::Column::Type::String, L"Value", 150 }
            });
        _token_store.add(stats_box->on_item_selected += [&](const auto& item)
        {
            switch (_selected_type)
            {
            case Waypoint::Type::Entity:
                on_item_selected(_all_items[_selected_index]);
                break;
            case Waypoint::Type::Trigger:
                on_trigger_selected(_all_triggers[_selected_index]);
                break;
            }
        });
        _stats = group_box->add_child(std::move(stats_box));
        right_panel->add_child(std::move(group_box));

        // Notes area.
        auto notes_box = std::make_unique<GroupBox>(Point(), Size(panel_width, window().size().height - 110), Colours::Notes, Colours::DetailsBorder, L"Notes");

        auto notes_area = std::make_unique<TextArea>(Point(10, 21), Size(200, 200), Colour(0.2f, 0.2f, 0.2f), Colour(1.0f, 1.0f, 1.0f));
        _notes_area = notes_box->add_child(std::move(notes_area));

        right_panel->add_child(std::make_unique<ui::Window>(Point(), Size(panel_width, 5), Colours::Notes));
        right_panel->add_child(std::move(notes_box));

        return right_panel;
    }

    void RouteWindow::load_waypoint_details(uint32_t index)
    {
        const auto& waypoint = _all_waypoints[index];
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", waypoint_type_to_string(waypoint.type())));
        stats.push_back(make_item(L"Position", pos_to_string(waypoint.position())));

        _selected_type = waypoint.type();
        _selected_index = waypoint.index();

        if (waypoint.type() != Waypoint::Type::Position)
        {
            stats.push_back(make_item(L"Target Index", std::to_wstring(waypoint.index())));
            if (waypoint.type() == Waypoint::Type::Entity)
            {
                stats.push_back(make_item(L"Entity", _all_items[waypoint.index()].type()));
            }
            else if (waypoint.type() == Waypoint::Type::Trigger)
            {
                stats.push_back(make_item(L"Trigger Type", trigger_type_name(_all_triggers[waypoint.index()]->type())));
            }
        }

        _stats->set_items(stats);
    }

    void RouteWindow::select_waypoint(uint32_t index)
    {
        _waypoints->set_selected_item(create_listbox_item(index, _all_waypoints[index]));
        load_waypoint_details(index);
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
