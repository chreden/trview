#include "ContextMenu.h"
#include <trview.ui/Button.h>
#include <trview.ui/Label.h>

using namespace trview::ui;

namespace trview
{
    ContextMenu::ContextMenu(Control& parent)
    {
        _menu = parent.add_child(std::make_unique<StackPanel>(Point(300, 300), Size(200, 100), Colour(0.15f, 0.15f, 0.15f), Size(1,1)));

        // Add a button in a little area with some writing.
        auto button_area = std::make_unique<StackPanel>(Point(), Size(100, 20), Colour(0.2f, 0.2f, 0.2f), Size(), StackPanel::Direction::Horizontal);
        auto button = std::make_unique<Button>(Point(), Size(24, 24), L"+");
        button->on_click += on_add_waypoint;
        button_area->add_child(std::move(button));
        button_area->add_child(std::make_unique<Label>(Point(), Size(100, 24), Colour(0.2f, 0.2f, 0.2f), L"Add Waypoint", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
        _menu->add_child(std::move(button_area));

        // Add the similar remove waypoint button 
        auto remove_button_area = std::make_unique<StackPanel>(Point(), Size(100, 20), Colour(0.2f, 0.2f, 0.2f), Size(), StackPanel::Direction::Horizontal);
        auto remove_button = std::make_unique<Button>(Point(), Size(24, 24), L"-");
        remove_button->on_click += on_remove_waypoint;
        remove_button_area->add_child(std::move(remove_button));
        auto remove_label = std::make_unique<Label>(Point(), Size(100, 24), Colour(0.2f, 0.2f, 0.2f), L"Remove Waypoint", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        _remove_label = remove_button_area->add_child(std::move(remove_label));
        _menu->add_child(std::move(remove_button_area));

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
        _remove_label->set_text_colour(value ? Colour(1.0f, 1.0f, 1.0f) : Colour(0.4f, 0.4f, 0.4f));
    }
}
