#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Event.h>

namespace trview
{
    class ViewMenu final : public MessageHandler
    {
    public:
        explicit ViewMenu(HWND window);
        virtual ~ViewMenu() = default;
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when the show minimap option is toggled.
        Event<bool> on_show_minimap;

        /// Event raised when the show tooltip option is toggled.
        Event<bool> on_show_tooltip;

        /// Event raised when the show UI option is toggled.
        Event<bool> on_show_ui;
    };
}
