#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace trview
{
    enum class Op
    {
        And,
        Or
    };

    template <typename T>
    class Filters
    {
    public:
        /// <summary>
        /// Function that will return the value from a subject as a string.
        /// </summary>
        using ValueGetter = std::function<std::string (const T&)>;

        struct Filter
        {
            std::string key;
            std::string value;

            
            Op op{ Op::And };
        };
        std::vector<Filter> filters;

        /// <summary>
        /// Add a getter definition to extract values from an object.
        /// </summary>
        /// <param name="key">The key used in filters</param>
        /// <param name="getter">The getter function.</param>
        void add_getter(const std::string& key, const ValueGetter& getter);
        std::vector<std::string> keys() const;
        /// <summary>
        /// Check whether the object matches the configured filters.
        /// </summary>
        /// <param name="value">The object to test.</param>
        /// <returns>Whether it was a match.</returns>
        bool match(const T& value) const;
    private:
        std::unordered_map<std::string, ValueGetter> _getters;
    };

    constexpr std::string op_to_string(Op op);
}

#include "Filters.hpp"
