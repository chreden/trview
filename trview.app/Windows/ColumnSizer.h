#pragma once

#include <cstdint>
#include <string>

namespace trview
{
    class ColumnSizer final
    {
    public:
        /// <summary>
        /// Add a string to the rolling measure for a column.
        /// </summary>
        /// <param name="value">String to measure.</param>
        /// <param name="column">Column index.</param>
        void measure(const std::string& value, uint32_t column);
        void reset();
        /// <summary>
        /// Get the calculated size for all column data.
        /// </summary>
        /// <returns>The final size.</returns>
        float size() const;
        /// <summary>
        /// Get the calculated size for a single column.
        /// </summary>
        /// <param name="index">The column index.</param>
        /// <returns>Calculated size.</returns>
        float size(uint32_t index) const;
    private:
        std::vector<float> _sizes;
    };
}

