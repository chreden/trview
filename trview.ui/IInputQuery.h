#pragma once

namespace trview
{
    namespace ui
    {
        class Control;

        struct IInputQuery
        {
            virtual ~IInputQuery() = 0;
            virtual Control* focus_control() const = 0;
        };
    }
}