export module trview.common.size;

namespace trview
{
    /// A two dimensional size.
    export struct Size
    {
        /// Create a new size with width and height initialised to zero.
        Size() : Size(0, 0)
        {
        }

        /// Create a new size with the specified values.
        /// @param width The width dimension.
        /// @param height The height dimension.
        Size(float width, float height)
            : width(width), height(height)
        {
        }

        /// Add another size to this size.
        /// @param other The size to add to this size.
        /// @returns The resultant size.
        Size& operator+=(const Size& other)
        {
            width += other.width;
            height += other.height;
            return *this;
        }

        /// Subtract a size from this size.
        /// @param other The size to subtract from this size.
        /// @returns The resultant size.
        Size operator-(const Size& other) const
        {
            return Size(width - other.width, height - other.height);
        }

        /// Add another size to this size.
        /// @param other The size to add to this size.
        /// @returns The new size.
        Size operator+(const Size& other) const
        {
            return Size(width + other.width, height + other.height);
        }

        /// Divide the size by a divisor.
        /// @param The divisor.
        /// @returns The new size.
        Size operator/(float divisor) const
        {
            return Size(width / divisor, height / divisor);
        }

        /// Determines whether two sizes are equal.
        /// @param size The size to compare.
        bool operator==(const Size& size) const
        {
            return width == size.width && height == size.height;
        }

        /// Determines whether two sizes are different.
        /// @param size The size to compare.
        bool operator!=(const Size& size) const
        {
            return !operator==(size);
        }

        float width, height;
    };
}