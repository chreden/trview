#pragma once

#include <trview.common/Event.h>

namespace trview
{
    class Console final
    {
    public:
        struct Names
        {
            static const inline std::string log = "##Log";
            static const inline std::string input = "##input";
        };

        void render();
        bool visible() const;
        void print(const std::string& text);
        void set_visible(bool value);
        Event<std::string> on_command;
    private:
        std::array<char, 4096> _buffer;
        std::string _text;
        bool _visible{ false };
        bool _need_focus{ false };
    };
}
