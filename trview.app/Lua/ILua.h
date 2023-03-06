#pragma once

namespace trview
{
    struct IApplication;

    struct ILua
    {
        virtual ~ILua() = 0;
        virtual void do_file(const std::string& file) = 0;
        virtual void execute(const std::string& command) = 0;
        virtual void initialise(IApplication* application) = 0;

        Event<std::string> on_print;
    };
}
