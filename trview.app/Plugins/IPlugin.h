#pragma once

namespace trview
{
    struct IApplication;

    struct IPlugin
    {
        using Source = std::function<std::unique_ptr<IPlugin>(const std::string& directory)>;

        virtual ~IPlugin() = 0;
        virtual std::string name() const = 0;
        virtual std::string author() const = 0;
        virtual std::string description() const = 0;
        virtual void initialise(IApplication* application) = 0;
    };
}
