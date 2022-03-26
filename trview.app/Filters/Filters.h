#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

namespace trview
{
    enum class CompareOp
    {
        Equal,
        NotEqual,
        GreaterThan,
        GreaterThanOrEqual,
        LessThan,
        LessThanOrEqual,
        Between,
        BetweenInclusive,
        Exists
    };

    enum class Op
    {
        And,
        Or
    };

    template <typename T>
    class Filters
    {
    public:
        struct Names
        {
            static const inline std::string Popup{ "Filters" };
            static const inline std::string FiltersButton{ "Filters##FiltersButton" };
            static const inline std::string Enable{ "##filter_enabled" };
            static const inline std::string AddFilter{ "+" };
            static const inline std::string FilterKey{ "##filter-key-" };
            static const inline std::string FilterCompareOp{ "##filter-compare-op-" };
            static const inline std::string FilterValue{ "##filter-value-" };
            static const inline std::string RemoveFilter{ "X##filter-remove-" };
            static const inline std::string FilterOp{ "##filter-op-" };
        };

        struct Filter
        {
            std::string key;
            CompareOp compare{ CompareOp::Equal };
            std::string value;
            std::string value2;
            Op op{ Op::And };

            int value_count() const;
        };

        /// <summary>
        /// Add a getter definition to extract a value from an object.
        /// </summary>
        /// <param name="key">The key used in filters</param>
        /// <param name="getter">The getter function.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::function<value_type (const T&)>& getter);
        /// <summary>
        /// Add a getter definition to extract a value from an object with specific options.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="options">List of possible options.</param>
        /// <param name="getter">The getter function.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::vector<std::string>& options, const std::function<value_type(const T&)>& getter);
        /// <summary>
        /// Add a getter definition to extract a value from an object with a predicate.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="getter">The getter function.</param>
        /// <param name="predicate">Predicate function to determine whether a specific object supports this getter.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::function<value_type(const T&)>& getter, const std::function<bool(const T&)>& predicate);
        /// <summary>
        /// Add a getter definition to extract a value from an object with specific options and a predicate.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="options">List of possible options.</param>
        /// <param name="getter">The getter function.</param>
        /// <param name="predicate">Predicate function to determine whether a specific object supports this getter.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::vector<std::string>& options, const std::function<value_type(const T&)>& getter, const std::function<bool(const T&)>& predicate);
        /// <summary>
        /// Add a getter definition to extract multiple values from an object.
        /// </summary>
        /// <param name="key">The key used in filters</param>
        /// <param name="getter">The getter function.</param>
        template <typename value_type>
        void add_multi_getter(const std::string& key, const std::function<std::vector<value_type> (const T&)>& getter);
        /// <summary>
        /// Add a getter definition to extract multiple values from an object with specific options.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="options">List of possible options.</param>
        /// <param name="getter">The getter function.</param>
        template <typename value_type>
        void add_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<value_type>(const T&)>& getter);
        /// <summary>
        /// Add a getter definition to extract multiple values from an object with a predicate.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="getter">The getter function.</param>
        /// <param name="predicate">Predicate function to determine whether a specific object supports this getter.</param>
        template <typename value_type>
        void add_multi_getter(const std::string& key, const std::function<std::vector<value_type>(const T&)>& getter, const std::function<bool(const T&)>& predicate);
        /// <summary>
        /// Add a getter definition to extract multiple values from an object with specific options and a predicate.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="options">List of possible options.</param>
        /// <param name="getter">The getter function.</param>
        /// <param name="predicate">Predicate function to determine whether a specific object supports this getter.</param>
        template <typename value_type>
        void add_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<value_type>(const T&)>& getter, const std::function<bool(const T&)>& predicate);
        /// <summary>
        /// Remove all getters and multi getters
        /// </summary>
        void clear_all_getters();
        /// <summary>
        /// Check whether the object matches the configured filters.
        /// </summary>
        /// <param name="value">The object to test.</param>
        /// <returns>Whether it was a match.</returns>
        bool match(const T& value) const;
        /// <summary>
        /// Render the filters button and popup.
        /// </summary>
        void render();
        /// <summary>
        /// Set the filters to a specific value.
        /// </summary>
        /// <param name="filters">The filters to use.</param>
        void set_filters(const std::vector<Filter> filters);
    private:
        using Value = std::variant<std::string, float, bool>;

        template <typename return_type>
        struct Getter
        {
            std::vector<CompareOp> ops;
            std::vector<std::string> options;
            std::function<return_type (const T&)> function;
            std::function<bool(const T&)> predicate;
        };

        /// <summary>
        /// Function that will return the value from a subject as a string.
        /// </summary>
        using ValueGetter = Getter<Value>;
        /// <summary>
        /// Function that will return multiple values from a subject as several strings.
        /// </summary>
        using MultiGetter = Getter<std::vector<Value>>;

        std::vector<Filter> _filters;
        std::vector<std::string> keys() const;
        /// <summary>
        /// Returns whether there are any filters of consequence.
        /// </summary>
        /// <returns>True if there's nothing of consequence.</returns>
        bool empty() const;
        void toggle_visible();
        bool is_match(const Value& value, const Filter& filter) const;
        bool is_match(const std::string& value, const Filter& filter) const;
        bool is_match(float value, const Filter& filter) const;
        bool is_match(bool value, const Filter& filter) const;
        std::vector<CompareOp> ops_for_key(const std::string& key) const;
        std::vector<std::string> options_for_key(const std::string& key) const;
        bool has_options(const std::string& key) const;
        std::unordered_map<std::string, ValueGetter> _getters;
        std::unordered_map<std::string, MultiGetter> _multi_getters;
        bool _show_filters{ false };
        bool _enabled{ true };
    };

    constexpr std::string to_string(CompareOp op);
    constexpr std::string to_string(Op op);

    /// <summary>
    /// Get the <see cref="CompareOp" />s that a type supports.
    /// </summary>
    /// <typeparam name="T">The type to check.</typeparam>
    /// <returns>Supported <see cref="CompareOp"/>s</returns>
    template <typename T>
    constexpr std::vector<CompareOp> compare_ops();

    /// <summary>
    /// Get the acceptable values for a type.
    /// </summary>
    /// <typeparam name="T">Type to check.</typeparam>
    /// <returns>The acceptable options. Emtpy means that there are no restrictions.</returns>
    template <typename T>
    constexpr std::vector<std::string> available_options();
}

#include "Filters.hpp"
