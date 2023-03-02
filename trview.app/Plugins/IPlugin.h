#pragma once

namespace trview
{
    struct IPlugin
    {
        using Source = std::function<std::unique_ptr<IPlugin>(const std::string& directory)>;

        virtual ~IPlugin() = 0;
        virtual std::string name() const = 0;
        virtual std::string author() const = 0;
        virtual std::string description() const = 0;
    };
}
