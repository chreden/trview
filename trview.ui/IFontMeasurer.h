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

            /// Determines if the character is in the image set.
            /// @param character The character to test.
            /// @returns True if the character has an image.
            virtual bool is_valid_character(wchar_t character) const = 0;
        };
    }
}
