#pragma once

namespace trview
{
    struct IPlugin
    {
        using Source = std::function<std::unique_ptr<IPlugin>(const std::string& directory)>;

        virtual ~IPlugin() = 0;
    };
}
