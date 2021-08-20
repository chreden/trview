#include "ContextMenu.h"
#include <trview.ui/Button.h>
#include <trview.ui/Label.h>
#include <trview.ui/Layouts/StackLayout.h>

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

    const std::string ContextMenu::Names::hide_button{ "Hide" };
    const std::string ContextMenu::Names::orbit_button{ "Orbit" };
    const std::string ContextMenu::Names::add_waypoint_button{ "AddWaypoint" };
    const std::string ContextMenu::Names::add_mid_waypoint_button{ "AddMidWaypoint" };
    const std::string ContextMenu::Names::remove_waypoint_button{ "RemoveWaypoint" };

    ContextMenu::ContextMenu(Control& parent)
    {
        _menu = parent.add_child(std::make_unique<ui::Window>(Point(300, 300), Size(200, 100), Colours::Background));
        _menu->set_layout(std::make_unique<StackLayout>(Size()));

        // Add waypoint button.
        auto button = _menu->add_child(std::make_unique<Button>(Size(100, 24), L"Add Waypoint"));
        button->set_name(Names::add_waypoint_button);
        button->set_text_background_colour(Colours::Button);
        _token_store += button->on_click += [&]()
        {
            on_add_waypoint();
            set_visible(false);
        };

        _mid_button = _menu->add_child(std::make_unique<Button>(Size(100, 24), L"Add Mid Waypoint"));
        _mid_button->set_name(Names::add_mid_waypoint_button);
        _mid_button->set_text_background_colour(Colours::Button);
        _token_store += _mid_button->on_click += [&]()
        {
            if (_mid_enabled)
            {
                on_add_mid_waypoint();
                set_visible(false);
            }
        };

        // Add the similar remove waypoint button 
        _remove_button = _menu->add_child(std::make_unique<Button>(Size(100, 24), L"Remove Waypoint"));
        _remove_button->set_name(Names::remove_waypoint_button);
        _remove_button->set_text_background_colour(Colours::Button);
        _token_store += _remove_button->on_click += [&]()
        {
            if (_remove_enabled)
            {
                on_remove_waypoint();
                set_visible(false);
            }
        };

        auto orbit_button = _menu->add_child(std::make_unique<Button>(Size(100, 24), L"Orbit Here"));
        orbit_button->set_name(Names::orbit_button);
        orbit_button->set_text_background_colour(Colours::Button);
        _token_store += orbit_button->on_click += [&]()
        {
            on_orbit_here();
            set_visible(false);
        };

        _hide_button = _menu->add_child(std::make_unique<Button>(Size(100, 24), L"Hide"));
        _hide_button->set_name(Names::hide_button);
        _hide_button->set_text_background_colour(Colours::Button);
        _token_store += _hide_button->on_click += [&]()
        {
            if (_hide_enabled)
            {
                on_hide();
                set_visible(false);
            }
        };

        set_remove_enabled(_remove_enabled);
        set_hide_enabled(_hide_enabled);
        set_mid_waypoint_enabled(_mid_enabled);

        _menu->set_visible(false);
    }

    const ui::Control* ContextMenu::control() const
    {
        return _menu;
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

    void ContextMenu::set_hide_enabled(bool value)
    {
        _hide_enabled = value;
        _hide_button->set_text_colour(value ? Colours::Enabled : Colours::Disabled);
    }

    bool ContextMenu::visible() const
    {
        return _menu->visible();
    }

    void ContextMenu::set_mid_waypoint_enabled(bool value)
    {
        _mid_enabled = value;
        _mid_button->set_text_colour(value ? Colours::Enabled : Colours::Disabled);
    }
}
