module;

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/misc/cpp/imgui_stdlib.h>

export module trview.app:Filters;

import std;

import trview.common;

import :IFilterable;
import :RowCounter;
import :Modal;
import :imgui;
import :Forward;

namespace trview
{
    export enum class CompareOp
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
        EndsWith,
        Matches
    };

    export enum class Op
    {
        And,
        Or
    };

    export enum class EditMode
    {
        Read,
        ReadWrite
    };

    export constexpr std::string to_string(CompareOp op) noexcept;
    export constexpr std::string to_string(Op op) noexcept;

    export class Filters
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

            bool operator == (const Filter&) const = default;
            auto operator <=> (const Filter&) const = default;

            bool empty() const;
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
            std::string type_key;
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
            GettersBuilder& with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change = EditMode::Read, const std::string& type_key = "");

            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter);
            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter);
            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate);
            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate);
            template <typename T, typename ValueType>
            GettersBuilder& with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate, const std::string& type_key);

            GettersBuilder& with_type_key(const std::string& key);

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

        Filters() = default;
        explicit Filters(const std::weak_ptr<IFilterStore>& filter_store);
        void add_filter(const Filter& filter);
        void add_getters(const Getters& getters);
        void clear_all_getters();
        std::vector<std::string> columns() const;
        std::vector<Filter> filters() const;
        void force_sort();
        bool group_match(std::ranges::input_range auto&& results, const Filter& filter) const;
        bool has_type_key(const std::string& type) const;
        bool is_match(const Value& value, const Filter& filter) const;
        bool is_match(const std::string& value, const Filter& filter) const;
        bool is_match(float value, const Filter& filter) const;
        bool is_match(bool value, const Filter& filter) const;
        bool is_match(std::weak_ptr<IFilterable> value, const Filter& filter) const;
        std::vector<std::string> keys(const std::string& type_key) const;
        bool match(const IFilterable& value) const;
        bool match(const Filter& filter, const IFilterable& value, const std::string& type_key) const;
        void render();
        void render_window();
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
        void set_name(const std::string& id);
    private:
        enum class Action
        {
            None,
            Remove
        };

        int column_count() const;
        const Getters& find_getter(const std::string& type_key) const;
        bool has_options(const std::string& type_key, const std::string& key) const;
        std::vector<CompareOp> compare_ops_for_key(const std::string& type_key, const std::string& key) const;
        std::vector<std::string> options_for_key(const std::string& type_key, const std::string& key) const;
        Action render(Filter& filter, int32_t depth, int32_t index, Filter& parent, const std::string& type_key)
        {
            const auto keys = this->keys(type_key);

            // For the 0th element we always just draw children.
            if (!filter.children.empty() || depth == 0 || filter.compare == CompareOp::Matches)
            {
                if (filter.compare == CompareOp::Matches)
                {
                    if (Action::Remove == render_leaf(filter, depth, index, type_key))
                    {
                        return Action::Remove;
                    }
                }

                const std::string suffix = std::format("{}-{}", depth, index);

                if (ImGui::BeginChild((std::string("FilterGroup##") + suffix).c_str(), ImVec2(), ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY))
                {
                    int32_t child_index = 0;
                    for (auto& child : filter.children)
                    {
                        const std::string child_suffix = std::format("{}-{}-{}", depth, index, child_index);
                        if (Action::Remove == render(child, depth + 1, child_index, filter, filter.type_key != "" ? filter.type_key : type_key))
                        {
                            filter.children.erase(filter.children.begin() + child_index);
                            break;
                        }

                        if (depth > 0)
                        {
                            ImGui::SameLine();
                            if (ImGui::Button((std::string("<##") + child_suffix).c_str()))
                            {
                                const auto filter_to_promote = child;
                                filter.children.erase(filter.children.begin() + child_index);

                                const auto filter_in_parent = std::ranges::find(parent.children, filter);
                                const auto filter_in_parent_index = filter_in_parent - parent.children.begin();

                                const bool last_filter_in_filter = filter.children.size() == 0;
                                parent.children.insert(filter_in_parent, filter_to_promote);

                                if (last_filter_in_filter)
                                {
                                    parent.children.erase(parent.children.begin() + filter_in_parent_index + 1);
                                }

                                break;
                            }
                            if (ImGui::IsItemHovered())
                            {
                                ImGui::SetTooltip("Move this condition into the parent condition");
                            }
                        }

                        ImGui::SameLine();
                        if (ImGui::Button((std::string(">##") + child_suffix).c_str()))
                        {
                            auto filter_to_group = child;
                            child = {};
                            child.children.push_back(filter_to_group);
                            break;
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::SetTooltip("Make this condition a child of the current condition");
                        }

                        if (child_index != filter.children.size() - 1)
                        {
                            std::vector<Op> ops{ Op::And, Op::Or };
                            if (ImGui::BeginCombo((Names::FilterOp + child_suffix).c_str(), to_string(child.op).c_str()))
                            {
                                for (const auto& op : ops)
                                {
                                    if (ImGui::Selectable(to_string(op).c_str(), op == child.op))
                                    {
                                        child.op = op;
                                        _changed = true;
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }
                        }
                        ++child_index;
                    }

                    if (ImGui::Button(std::format("{}##{}", Names::AddFilter, suffix).c_str()))
                    {
                        _changed = true;
                        filter.children.push_back({});
                    }
                }

                ImGui::EndChild();
            }
            else
            {
                return render_leaf(filter, depth, index, type_key);
            }

            return Action::None;
        }

        Action render_leaf(Filter& filter, int32_t depth, int32_t index, const std::string& type_key);
        void render_menu_bar();
        void render_filter_name_modal();
        void render_filters();

        bool                        _changed{ true };
        std::vector<std::string>    _columns;
        std::vector<std::size_t>    _column_order;
        bool                        _enabled{ true };
        Filter                      _filter;
        std::vector<Getters>        _getters;
        mutable bool                _force_sort{ false };
        mutable bool                _scroll_to_item{ false };
        bool                        _show_filters{ false };
        std::weak_ptr<IFilterStore> _filter_store;
        std::string                 _id;
        std::string                 _name;

        struct ModalState
        {
            std::string name_value;
        };
        Modal<ModalState> _save_modal;
    };

    /// <summary>
    /// Get the <see cref="CompareOp" />s that a type supports.
    /// </summary>
    /// <typeparam name="T">The type to check.</typeparam>
    /// <returns>Supported <see cref="CompareOp"/>s</returns>
    export template <typename T>
    constexpr std::vector<CompareOp> compare_ops() noexcept;

    /// <summary>
    /// Get the acceptable values for a type.
    /// </summary>
    /// <typeparam name="T">Type to check.</typeparam>
    /// <returns>The acceptable options. Emtpy means that there are no restrictions.</returns>
    export template <typename T>
    constexpr std::vector<std::string> available_options() noexcept;

    export template <typename T>
    std::unordered_map<std::string, Filters::Toggle> default_hide(const std::vector<std::shared_ptr<T>>& filtered_entries);
}

#include "Filters.hpp"
