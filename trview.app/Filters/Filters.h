#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <variant>
#include <ranges>

#include "../Windows/RowCounter.h"
#include "IFilterable.h"

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
        Exists,
        NotExists,
        StartsWith,
        EndsWith
    };

    enum class Op
    {
        And,
        Or
    };

    enum class EditMode
    {
        Read,
        ReadWrite
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
            std::vector<Filter> children;
            Op op{ Op::And };
            bool invert{ false };

            auto operator <=> (const Filter&) const = default;

            int value_count() const noexcept;
            bool initial_state() const noexcept;
        };

        struct Toggle
        {
            std::function<void(std::weak_ptr<T>, bool)> on_toggle;
            std::function<void(bool)> on_toggle_all;
            std::function<bool()> all_toggled;
        };

        Event<> on_columns_reset;
        Event<> on_columns_saved;

        void add_filter(const Filter& filter);

        /// <summary>
        /// Add a getter definition to extract a value from an object.
        /// </summary>
        /// <param name="key">The key used in filters</param>
        /// <param name="getter">The getter function.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::function<value_type (const T&)>& getter, EditMode can_change = EditMode::Read);
        /// <summary>
        /// Add a getter definition to extract a value from an object with specific options.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="options">List of possible options.</param>
        /// <param name="getter">The getter function.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::vector<std::string>& options, const std::function<value_type(const T&)>& getter, EditMode can_change = EditMode::Read);
        /// <summary>
        /// Add a getter definition to extract a value from an object with a predicate.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="getter">The getter function.</param>
        /// <param name="predicate">Predicate function to determine whether a specific object supports this getter.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::function<value_type(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change = EditMode::Read);
        /// <summary>
        /// Add a getter definition to extract a value from an object with specific options and a predicate.
        /// </summary>
        /// <param name="key">The key used in filters.</param>
        /// <param name="options">List of possible options.</param>
        /// <param name="getter">The getter function.</param>
        /// <param name="predicate">Predicate function to determine whether a specific object supports this getter.</param>
        template <typename value_type>
        void add_getter(const std::string& key, const std::vector<std::string>& options, const std::function<value_type(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change = EditMode::Read);
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
        void force_sort();
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
        void render_settings();
        /// <summary>
        /// Render the entity list.
        /// </summary>
        /// <param name="items">Filtered items.</param>
        /// <param name="all_items">All items.</param>
        /// <param name="selected_item">The currently selected item.</param>
        void render_table(
            const std::ranges::forward_range auto& items,
            std::ranges::forward_range auto& all_items,
            const std::weak_ptr<T>& selected_item,
            RowCounter counter,
            const std::function<void(std::weak_ptr<T>)>& on_item_selected,
            const std::unordered_map<std::string, Toggle>& on_toggle);
        void scroll_to_item();
        void set_columns(const std::vector<std::string>& columns);
        /// <summary>
        /// Set the filters to a specific value.
        /// </summary>
        /// <param name="filters">The filters to use.</param>
        void set_filters(const std::vector<Filter> filters);
        /// <summary>
        /// Test whether filters were changed and reset the changed flag.
        /// </summary>
        /// <returns>Whether filters were changed.</returns>
        bool test_and_reset_changed();
        std::vector<std::string> columns() const;
        std::vector<Filter> filters() const;
    private:
        int column_count() const;
        bool match(const Filter& filter, const T& value) const;

        enum class Action
        {
            None,
            Remove
        };

        Action render(Filter& filter, const std::vector<std::string>& keys, int32_t depth, int32_t index, Filter& parent);

        using Value = std::variant<std::string, float, bool, int>;

        template <typename return_type>
        struct Getter
        {
            std::vector<CompareOp> ops;
            std::vector<std::string> options;
            std::function<return_type (const T&)> function;
            std::function<bool(const T&)> predicate;
            EditMode can_change{ EditMode::Read };
        };

        /// <summary>
        /// Function that will return the value from a subject as a string.
        /// </summary>
        using ValueGetter = Getter<Value>;
        /// <summary>
        /// Function that will return multiple values from a subject as several strings.
        /// </summary>
        using MultiGetter = Getter<std::vector<Value>>;

        Filter _filter;
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
        bool group_match(std::ranges::input_range auto&& results, const Filter& filter) const;
        std::vector<CompareOp> ops_for_key(const std::string& key) const;
        std::vector<std::string> options_for_key(const std::string& key) const;
        bool has_options(const std::string& key) const;
        std::map<std::string, ValueGetter> _getters;
        std::map<std::string, MultiGetter> _multi_getters;
        bool _show_filters{ false };
        bool _enabled{ true };
        bool _changed{ true };
        mutable bool _scroll_to_item{ false };
        mutable bool _force_sort{ false };
        std::vector<std::string> _columns;
        std::vector<std::size_t> _column_order;
    };

    class Filters2
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

        using Value = std::variant<std::string, float, bool, int, std::weak_ptr<IFilterable>>;
        using TypeMatcher = std::function<bool(const std::weak_ptr<IFilterable>&)>;

        struct Filter
        {
            std::string key;
            CompareOp compare{ CompareOp::Equal };
            std::string value;
            std::string value2;
            std::vector<Filter> children;
            Op op{ Op::And };
            bool invert{ false };
            std::string type_key;

            auto operator <=> (const Filter&) const = default;

            int value_count() const noexcept;
            bool initial_state() const noexcept;
        };

        template <typename return_type>
        struct Getter
        {
            std::vector<CompareOp> ops;
            std::vector<std::string> options;
            std::function<return_type(const IFilterable&)> function;
            std::function<bool(const IFilterable&)> predicate;
            EditMode can_change{ EditMode::Read };
        };

        /// <summary>
        /// Function that will return the value from a subject as a string.
        /// </summary>
        using ValueGetter = Getter<Value>;
        /// <summary>
        /// Function that will return multiple values from a subject as several strings.
        /// </summary>
        using MultiGetter = Getter<std::vector<Value>>;

        struct Getters
        {
            std::string type_key;
            TypeMatcher type_matcher;
            std::map<std::string, ValueGetter> getters;
            std::map<std::string, MultiGetter> multi_getters;
        };

        class GettersBuilder
        {
        public:
            template <typename T, typename ValueType>
            GettersBuilder& with_getter(const std::string& key, const std::function<ValueType (const T&)>& getter, EditMode can_change = EditMode::Read);
            template <typename T, typename ValueType>
            GettersBuilder& with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, EditMode can_change = EditMode::Read);
            template <typename T, typename ValueType>
            GettersBuilder& with_getter(const std::string& key, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change = EditMode::Read);
            template <typename T, typename ValueType>
            GettersBuilder& with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change = EditMode::Read);

            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter);
            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter);
            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate);
            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate);

            GettersBuilder& with_type_key(const std::string& key);

            template <typename T>
            Getters build() const;
        private:
            std::string _type_key;
            std::map<std::string, ValueGetter> _getters;
            std::map<std::string, MultiGetter> _multi_getters;
        };

        struct Toggle
        {
            std::function<void(std::weak_ptr<IFilterable>, bool)> on_toggle;
            std::function<void(bool)> on_toggle_all;
            std::function<bool()> all_toggled;
        };

        Event<> on_columns_reset;
        Event<> on_columns_saved;

        void add_getters(const Getters& getters);
        void clear_all_getters();
        std::vector<std::string> columns() const;
        void force_sort();
        bool group_match(std::ranges::input_range auto&& results, const Filter& filter) const;
        bool is_match(const Value& value, const Filter& filter) const;
        bool is_match(const std::string& value, const Filter& filter) const;
        bool is_match(float value, const Filter& filter) const;
        bool is_match(bool value, const Filter& filter) const;
        std::vector<std::string> keys(const std::string& type_key) const;
        bool match(const IFilterable& value) const;
        bool match(const Filters2::Filter& filter, const IFilterable& value, const std::string& type_key) const;
        void render();
        void render_settings();
        void render_table(const std::ranges::forward_range auto& items,
            std::ranges::forward_range auto& all_items,
            const std::weak_ptr<IFilterable>& selected_item,
            RowCounter counter,
            const std::function<void(std::weak_ptr<IFilterable>)>& on_item_selected,
            const std::unordered_map<std::string, Toggle>& on_toggle);
        void set_columns(const std::vector<std::string>& columns);
        void set_filters(const std::vector<Filter> filters);
        void set_type_key(const std::string& type_key);
        void scroll_to_item();
        bool test_and_reset_changed();
        void toggle_visible();
    private:
        enum class Action
        {
            None,
            Remove
        };

        int column_count() const;
        const Getters& find_getter(const std::string& type_key) const;
        bool has_options(const std::string& type_key, const std::string& key) const;
        std::vector<CompareOp> ops_for_key(const std::string& type_key, const std::string& key) const;
        std::vector<std::string> options_for_key(const std::string& type_key, const std::string& key) const;
        Action render(Filter& filter, const std::vector<std::string>& keys, int32_t depth, int32_t index, Filter& parent, const std::string& type_key);

        bool                     _changed{ true };
        std::vector<std::string> _columns;
        std::vector<std::size_t> _column_order;
        bool                     _enabled{ true };
        Filter                   _filter;
        std::vector<Getters>     _getters;
        mutable bool             _force_sort{ false };
        mutable bool             _scroll_to_item{ false };
        bool                     _show_filters{ false };
    };

    constexpr std::string to_string(CompareOp op) noexcept;
    constexpr std::string to_string(Op op) noexcept;

    /// <summary>
    /// Get the <see cref="CompareOp" />s that a type supports.
    /// </summary>
    /// <typeparam name="T">The type to check.</typeparam>
    /// <returns>Supported <see cref="CompareOp"/>s</returns>
    template <typename T>
    constexpr std::vector<CompareOp> compare_ops() noexcept;

    /// <summary>
    /// Get the acceptable values for a type.
    /// </summary>
    /// <typeparam name="T">Type to check.</typeparam>
    /// <returns>The acceptable options. Emtpy means that there are no restrictions.</returns>
    template <typename T>
    constexpr std::vector<std::string> available_options() noexcept;

    template <typename T>
    std::unordered_map<std::string, typename Filters<T>::Toggle> default_hide(const std::vector<std::shared_ptr<T>>& filtered_entries);

    template <typename T>
    std::unordered_map<std::string, Filters2::Toggle> default_hide2(const std::vector<std::shared_ptr<T>>& filtered_entries);
}

#include "Filters.hpp"
