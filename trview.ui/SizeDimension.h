/// @file SizeDimension.h
/// @brief Describes how a control should resize.

#pragma once

namespace trview
{
    namespace ui
    {
        /// <summary>
        /// Describes how a control should resize.
        /// </summary>
        enum class SizeDimension
        {
            /// <summary>
            /// The control should not resize
            /// </summary>
            None,
            /// <summary>
            /// The control should resize its width only.
            /// </summary>
            Width,
            /// <summary>
            /// The control should resize its height only.
            /// </summary>
            Height,
            /// <summary>
            /// The control should resize its width and height.
            /// </summary>
            All
        };
    }
}