#pragma once

#include "IShortcuts.h"
#include "../MessageHandler.h"
#include "../Event.h"

namespace trview
{
    class Shortcuts : public IShortcuts, public MessageHandler
    {
    public:
        explicit Shortcuts(const Window& window);
        virtual ~Shortcuts() = default;
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        virtual Event<>& add_shortcut(bool control, uint16_t key) override;

        virtual std::vector<Shortcut> shortcuts() const override;
    private:
        void create_accelerators();

        HACCEL _accelerators;
        std::vector<std::pair<Shortcut, Event<>>> _shortcuts;
        uint16_t _command_index;
    };
}
