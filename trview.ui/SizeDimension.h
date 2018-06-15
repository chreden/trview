/// @file SizeDimension.h
/// @brief Describes how a control should resize.

#pragma once

namespace trview
{
    namespace ui
    {
        /// Describes how a control should resize.
        enum class SizeDimension
        {
            /// The control should resize its width only.
            Width,
            /// The control should resize its height only.
            Height,
            /// The control should resize its width and height.
            All
        };
    }
}