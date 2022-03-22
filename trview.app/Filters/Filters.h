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
        /// Add a getter definition to extract values from an object.
        /// </summary>
        /// <param name="key">The key used in filters</param>
        /// <param name="getter">The getter function.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::function<value_type (const T&)>& getter);

        template <typename value_type>
        void add_getter(const std::string& key, const std::function<value_type(const T&)>& getter, const std::function<bool(const T&)>& predicate);

        template <typename value_type>
        void add_multi_getter(const std::string& key, const std::function<std::vector<value_type> (const T&)>& getter);

        template <typename value_type>
        void add_multi_getter(const std::string& key, const std::function<std::vector<value_type>(const T&)>& getter, const std::function<bool(const T&)>& predicate);

        void clear_all_getters();
        /// <summary>
        /// Check whether the object matches the configured filters.
        /// </summary>
        /// <param name="value">The object to test.</param>
        /// <returns>Whether it was a match.</returns>
        bool match(const T& value) const;
        void render();
        void set_filters(const std::vector<Filter> filters);
    private:
        using Value = std::variant<std::string, float, bool>;
        /// <summary>
        /// Function that will return the value from a subject as a string.
        /// </summary>
        using ValueGetter = std::tuple<std::vector<CompareOp>, std::vector<std::string>, std::function<Value(const T&)>, std::function<bool(const T&)>>;
        /// <summary>
        /// Function that will return multiple values from a subject as several strings.
        /// </summary>
        using MultiGetter = std::tuple<std::vector<CompareOp>, std::vector<std::string>, std::function<std::vector<Value>(const T&)>, std::function<bool(const T&)>>;

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

    constexpr std::string compare_op_to_string(CompareOp op);
    constexpr std::string op_to_string(Op op);

    template <typename T>
    constexpr std::vector<CompareOp> compare_ops();

    template <typename T>
    constexpr std::vector<std::string> available_options();
}

#include "Filters.hpp"
