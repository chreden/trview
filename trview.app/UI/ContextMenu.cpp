#include "stdafx.h"
#include "ContextMenu.h"
#include <trview.ui/Button.h>
#include <trview.ui/Label.h>

using namespace trview::ui;

namespace trview
{
    namespace Colours
    {
        const Colour Background{ 0.15f, 0.15f, 0.15f };
        const Colour Button{ 0.2f, 0.2f, 0.2f };
        const Colour Enabled{ 1.0f, 1.0f, 1.0f };
        const Colour Disabled{ 0.4f, 0.4f, 0.4f };
    }

    ContextMenu::ContextMenu(Control& parent)
    {
        _menu = parent.add_child(std::make_unique<StackPanel>(Point(300, 300), Size(200, 100), Colours::Background, Size()));

        // Add waypoint button.
        auto button = std::make_unique<Button>(Size(100, 24), L"Add Waypoint");
        button->set_text_background_colour(Colours::Button);
        _token_store += button->on_click += [&]()
        {
            on_add_waypoint();
            set_visible(false);
        };
        _menu->add_child(std::move(button));

        // Add the similar remove waypoint button 
        auto remove_button = std::make_unique<Button>(Size(100, 24), L"Remove Waypoint");
        remove_button->set_text_background_colour(Colours::Button);
        _token_store += remove_button->on_click += [&]()
        {
            if (_remove_enabled)
            {
                on_remove_waypoint();
                set_visible(false);
            }
        };
        _remove_button = _menu->add_child(std::move(remove_button));

        auto orbit_button = std::make_unique<Button>(Size(100, 24), L"Orbit Here");
        orbit_button->set_text_background_colour(Colours::Button);
        _token_store += orbit_button->on_click += [&]()
        {
            on_orbit_here();
            set_visible(false);
        };
        _menu->add_child(std::move(orbit_button));

        _menu->set_visible(false);
    }

    void ContextMenu::set_position(const Point& position)
    {
        _menu->set_position(position);
    }

    void ContextMenu::set_visible(bool value)
    {
        _menu->set_visible(value);
    }

    void ContextMenu::set_remove_enabled(bool value)
    {
        _remove_enabled = value;
        _remove_button->set_text_colour(value ? Colours::Enabled : Colours::Disabled);
    }
}
