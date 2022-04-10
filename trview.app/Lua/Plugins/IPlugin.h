#pragma once

namespace trview
{
    struct IPlugin
    {
        using Source = std::function<std::unique_ptr<IPlugin>(const std::vector<uint8_t>&)>;

        virtual ~IPlugin() = 0;
        virtual void initialise() = 0;
        virtual void render() = 0;
        virtual void render_ui() = 0;
    };
}
