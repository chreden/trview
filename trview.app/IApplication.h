#pragma once

namespace trview
{
    struct IApplication
    {
        virtual ~IApplication() = 0;
        virtual int run() = 0;
        virtual void select_item(uint32_t index) = 0;
        Event<> on_closing;
    };
}
