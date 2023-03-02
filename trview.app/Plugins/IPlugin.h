#pragma once

namespace trview
{
    struct IPlugin
    {
        using Source = std::function<std::unique_ptr<IPlugin>(const std::vector<uint8_t>&)>;

        virtual ~IPlugin() = 0;
    };
}
