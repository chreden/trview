#pragma once

#include <string>
#include <trview.common/Event.h>

namespace trview
{
    struct IApplication;

    struct IPlugin
    {
        using Source = std::function<std::shared_ptr<IPlugin>(const std::string& directory)>;

        virtual ~IPlugin() = 0;
        virtual std::string name() const = 0;
        virtual std::string author() const = 0;
        virtual std::string description() const = 0;
        virtual void initialise(IApplication* application) = 0;
        virtual std::string path() const = 0;
        virtual std::string messages() const = 0;
        virtual void execute(const std::string& command) = 0;
        virtual void add_message(const std::string& message) = 0;
        virtual void do_file(const std::string& path) = 0;
        virtual void clear_messages() = 0;
        virtual void reload() = 0;
        virtual void render_toolbar() = 0;
        virtual void render_ui() = 0;

        Event<std::string> on_message;
    };
}
