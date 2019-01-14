/// @file IFontMeasurer.h
/// @brief Interface to allow the UI to find out what size a string will be when rendered.

#pragma once

#include <trview.common/Size.h>
#include <string>

namespace trview
{
    namespace ui
    {
        /// Interface to allow the UI to find out what size a string will be when rendered.
        struct IFontMeasurer
        {
            virtual ~IFontMeasurer() = 0;

            /// Measure the specified text.
            /// @param text The text to measure.
            virtual Size measure(const std::wstring& text) const = 0;
        };
    }
}
