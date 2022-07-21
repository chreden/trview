#pragma once

#include <string>
#include <trview.common/Logs/ILog.h>

namespace trview
{
    struct IPlugin
    {
        using Source = std::function<std::unique_ptr<IPlugin>(const std::string&, const std::vector<uint8_t>&)>;

        enum class Status
        {
            OK,
            Error
        };

        virtual ~IPlugin() = 0;
        virtual void initialise() = 0;
        virtual void render() = 0;
        virtual void render_ui() = 0;
    };
}
