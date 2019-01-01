#include "RouteWindow.h"
#include "Waypoint.h"
#include "Route.h"

namespace trview
{
    using namespace ui;

    namespace
    {
        Listbox::Item create_listbox_item(uint32_t index, const DirectX::SimpleMath::Vector3& waypoint)
        {
            return { {{ L"#", std::to_wstring(index) },
                     { L"Type", L"Unknown" }} };
        }
    }

    namespace Colours
    {
        const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
        const Colour LeftPanel{ 1.0f, 0.4f, 0.4f, 0.4f };
        const Colour ItemDetails{ 1.0f, 0.35f, 0.35f, 0.35f };
        const Colour Triggers{ 1.0f, 0.3f, 0.3f, 0.3f };
        const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
    }

    using namespace graphics;

    RouteWindow::RouteWindow(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.route", L"Route", Size(470, 400))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    void RouteWindow::load_waypoints(const Route& route) 
    {
        std::vector<Listbox::Item> items;
        for (uint32_t i = 0; i < route.waypoints(); ++i)
        {
            items.push_back(create_listbox_item(i, route.waypoint(i)));
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
            on_waypoint_selected(index);
        });
        _waypoints = left_panel->add_child(std::move(waypoints));
        return left_panel;
    }

    std::unique_ptr<Control> RouteWindow::create_right_panel()
    {
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Point(), Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        return right_panel;
    }
}
