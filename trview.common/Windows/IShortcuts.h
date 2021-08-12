#pragma once

#include "../Event.h"
#include "../Window.h"

namespace trview
{
    struct IShortcuts
    {
        using Source = std::function<std::shared_ptr<IShortcuts>(const Window&)>;

        struct Shortcut
        {
            uint8_t flags;
            uint16_t key;
            uint16_t command;
        };

        virtual ~IShortcuts() = 0;
        virtual Event<>& add_shortcut(bool control, uint16_t key) = 0;
        virtual std::vector<Shortcut> shortcuts() const = 0;
    };
}
