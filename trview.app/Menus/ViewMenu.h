#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Event.h>
#include <trview.common/Colour.h>

namespace trview
{
    class ViewMenu final : public MessageHandler
    {
    public:
        explicit ViewMenu(const Window& window);
        virtual ~ViewMenu() = default;
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when the show minimap option is toggled.
        Event<bool> on_show_minimap;

        /// Event raised when the show tooltip option is toggled.
        Event<bool> on_show_tooltip;

        /// Event raised when the show UI option is toggled.
        Event<bool> on_show_ui;

        /// Event raised when the show compass option is toggled.
        Event<bool> on_show_compass;

        /// Event raised when the show selection option is toggled.
        Event<bool> on_show_selection;

        /// Event raised when the show route option is toggled.
        Event<bool> on_show_route;

        /// Event raised when the show tools option is toggled.
        Event<bool> on_show_tools;

        /// Event raised when the background colour is changed.
        Event<Colour> on_colour_change; 

        /// Event raised when the unhide all option is selected.
        Event<> on_unhide_all;
    };
}
