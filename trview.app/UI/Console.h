#pragma once

#include <trview.common/Event.h>
#include <trview.common/IFiles.h>

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

        explicit Console(const std::shared_ptr<IFiles>& files);
        void render();
        bool visible() const;
        void print(const std::string& text);
        void set_visible(bool value);
        Event<std::string> on_command;
        void initialise_ui();
    private:
        std::array<char, 4096> _buffer;
        std::shared_ptr<IFiles> _files;
        std::string _text;
        bool _visible{ false };
        bool _need_focus{ false };
        ImFont* _font{ nullptr };
    };
}
