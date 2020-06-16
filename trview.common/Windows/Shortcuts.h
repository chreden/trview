#pragma once

#include "../MessageHandler.h"
#include "../Event.h"

namespace trview
{
    class Shortcuts : public MessageHandler
    {
    public:
        struct Shortcut
        {
            uint8_t flags;
            uint16_t key;
            uint16_t command;
        };

        explicit Shortcuts(const Window& window);
        virtual ~Shortcuts() = default;
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        Event<>& add_shortcut(bool control, uint16_t key);
    private:
        HACCEL _accelerators;
        std::vector<std::pair<Shortcut, Event<>>> _shortcuts;
        uint16_t _command_index;
    };
}
