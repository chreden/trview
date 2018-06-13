/// @file Size.h
/// @brief Class used to represent a size.
/// 
/// A floating point based two dimensional size.

#pragma once

namespace trview
{
    /// A two dimensional size.
    struct Size
    {
        /// Create a new size with width and height initialised to zero.
        Size();

        /// Create a new size with the specified values.
        /// @param width The width dimension.
        /// @param height The height dimension.
        Size(float width, float height);

        /// Add another size to this size.
        /// @param other The size to add to this size.
        /// @returns The resultant size.
        Size& operator+=(const Size& other);

        /// Add another size to this size.
        /// @param other The size to add to this size.
        /// @returns The new size.
        Size operator+(const Size& other) const;

        float width, height;
    };
}