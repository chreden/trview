#pragma once

#include <trview.common/Size.h>
#include <string>

namespace trview
{
    namespace ui
    {
        struct IFontMeasurer
        {
            virtual ~IFontMeasurer() = 0;

            /// Measure the specified text.
            /// @param text The text to measure.
            virtual Size measure(const std::wstring& text) const = 0;
        };
    }
}
