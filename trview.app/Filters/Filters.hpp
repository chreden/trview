#pragma once

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/misc/cpp/imgui_stdlib.h>
#include <charconv>
#include <ranges>

#include "../trview_imgui.h"

namespace trview
{
    constexpr std::string to_string(CompareOp op) noexcept
    {
        switch (op)
        {
        case CompareOp::Equal:
            return "is";
        case CompareOp::NotEqual:
            return "is not";
        case CompareOp::GreaterThan:
            return "greater than";
        case CompareOp::GreaterThanOrEqual:
            return "greater than or equal";
        case CompareOp::LessThan:
            return "less than";
        case CompareOp::LessThanOrEqual:
            return "less than or equal";
        case CompareOp::Between:
            return "between";
        case CompareOp::BetweenInclusive:
            return "between inclusive";
        case CompareOp::Exists:
            return "is present";
        case CompareOp::NotExists:
            return "is not present";
        case CompareOp::StartsWith:
            return "starts with";
        case CompareOp::EndsWith:
            return "ends with";
        case CompareOp::Matches:
            return "matches";
        }
        return "?";
    }

    constexpr std::string to_string(Op op) noexcept
    {
        switch (op)
        {
        case Op::And:
            return "And";
        case Op::Or:
            return "Or";
        }
        return "?";
    }

    template <typename T>
    constexpr std::vector<CompareOp> compare_ops() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
            CompareOp::StartsWith,
            CompareOp::EndsWith
        };
    }

    template <>
    constexpr std::vector<CompareOp> compare_ops<float>() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
            CompareOp::GreaterThan,
            CompareOp::GreaterThanOrEqual,
            CompareOp::LessThan,
            CompareOp::LessThanOrEqual,
            CompareOp::Between,
            CompareOp::BetweenInclusive
        };
    }

    template <>
    constexpr std::vector<CompareOp> compare_ops<bool>() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
        };
    }

    template <>
    constexpr std::vector<CompareOp> compare_ops<int>() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
            CompareOp::GreaterThan,
            CompareOp::GreaterThanOrEqual,
            CompareOp::LessThan,
            CompareOp::LessThanOrEqual,
            CompareOp::Between,
            CompareOp::BetweenInclusive
        };
    }

    template <>
    constexpr std::vector<CompareOp> compare_ops<std::weak_ptr<IFilterable>>() noexcept
    {
        return
        {
            CompareOp::Matches
        };
    }

    template <typename T>
    constexpr std::vector<std::string> available_options() noexcept
    {
        return {};
    }

    template <>
    constexpr std::vector<std::string> available_options<bool>() noexcept
    {
        return { "false", "true" };
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_getter(const std::string& key, const std::function<ValueType (const T&)>& getter, EditMode can_change)
    {
        return with_getter(key, available_options<ValueType>(), getter, {}, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, EditMode can_change)
    {
        return with_getter(key, options, getter, {}, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_getter(const std::string& key, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change)
    {
        return with_getter(key, available_options<ValueType>(), getter, predicate, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change)
    {
        return with_getter(key, options, getter, predicate, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change, const std::string& type_key)
    {
        ValueGetter new_getter
        {
            .ops = compare_ops<ValueType>(),
            .options = options,
            .function = [=](const IFilterable& f) -> Filters::Value
            {
                return getter(static_cast<const T&>(f));
            },
            .can_change = can_change,
            .type_key = type_key
        };

        if (predicate)
        {
            new_getter.predicate = [=](const IFilterable& f) -> bool
                {
                    return predicate(static_cast<const T&>(f));
                };
        }

        _getters[key] = new_getter;
        return *this;
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter)
    {
        return with_multi_getter(key, available_options<ValueType>(), getter, {});
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter)
    {
        return with_multi_getter(key, options, getter, {});
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate)
    {
        return with_multi_getter(key, available_options<ValueType>(), getter, predicate);
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate)
    {
        return with_multi_getter(key, options, getter, predicate, "");
    }

    template <typename T, typename ValueType>
    Filters::GettersBuilder& Filters::GettersBuilder::with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate, const std::string& type_key)
    {
        MultiGetter new_getter
        {
            .ops = compare_ops<ValueType>(),
            .options = options,
            .function = [=](const IFilterable& f) -> std::vector<Filters::Value>
            {
                return getter(static_cast<const T&>(f)) |
                    std::ranges::to<std::vector<Filters::Value>>();
            },
            .can_change = EditMode::Read,
            .type_key = type_key
        };

        if (predicate)
        {
            new_getter.predicate = [=](const IFilterable& f) -> bool
                {
                    return predicate(static_cast<const T&>(f));
                };
        }

        _multi_getters[key] = new_getter;
        return *this;
    }

    template <typename T>
    std::unordered_map<std::string, Filters::Toggle> default_hide(const std::vector<std::shared_ptr<T>>& filtered_entries)
    {
        return
        {
            {
                "Hide",
                {
                    .on_toggle = [&](auto&& entry, auto&& value)
                    {
                        if (auto entry_ptr = entry.lock())
                        {
                            auto t_ptr = std::static_pointer_cast<T>(entry_ptr);
                            t_ptr->set_visible(!value);
                        }
                    },
                    .on_toggle_all = [&](bool value)
                    {
                        std::ranges::for_each(filtered_entries, [=](auto&& entry) { entry->set_visible(!value); });
                    },
                    .all_toggled = [&]() { return std::ranges::all_of(filtered_entries, [](auto&& entry) { return !entry->visible(); }); }
                }
            }
        };
    }

    template < typename T >
    void imgui_sort_weak_filters(std::vector<std::weak_ptr<T>>& container, std::vector<std::function<bool(const IFilterable&, const IFilterable&)>> callbacks, bool force_sort = false)
    {
        container.erase(std::remove_if(container.begin(), container.end(), [](auto& e) { return e.lock() == nullptr; }), container.end());

        auto specs = ImGui::TableGetSortSpecs();
        if (specs && (specs->SpecsDirty || force_sort))
        {
            std::sort(container.begin(), container.end(),
                [&](const auto& l, const auto& r) -> bool
                {
                    const auto& callback = callbacks[specs->Specs[0].ColumnIndex];
                    const auto result = callback(*l.lock(), *r.lock());
                    return specs->Specs->SortDirection == ImGuiSortDirection_Ascending ? result : !result;
                });
            specs->SpecsDirty = false;
        }
    }

    void Filters::render_table(const std::ranges::forward_range auto& items,
        std::ranges::forward_range auto& all_items,
        const std::weak_ptr<IFilterable>& selected_item,
        RowCounter counter,
        const std::function<void(std::weak_ptr<IFilterable>)>& on_item_selected,
        const std::unordered_map<std::string, Toggle>& on_toggle)
    {
        auto columns = column_count();
        if (columns == 0)
        {
            return;
        }

        const auto& getters = find_getter(_filter.type_key);
        if (ImGui::BeginTable("filter-list", columns, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Reorderable, ImVec2(0, -counter.height())))
        {
            for (const auto& column_name : _columns)
            {
                ImGui::TableSetupColumn(column_name.c_str(), ImGuiTableColumnFlags_WidthFixed);
            }
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers, ImGui::TableGetHeaderRowHeight());

            int column_n = 0;
            for (const auto& column_name : _columns)
            {
                if (!ImGui::TableSetColumnIndex(column_n++))
                {
                    continue;
                }

                const auto found_getter = getters.getters.find(column_name);
                if (found_getter == getters.getters.end())
                {
                    continue;
                }
                const auto& getter = found_getter->second;
                if (getter.can_change == EditMode::ReadWrite)
                {
                    auto found_toggle = on_toggle.find(column_name);
                    if (found_toggle != on_toggle.end())
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        bool value = found_toggle->second.all_toggled();
                        if (ImGui::Checkbox("##checkall", &value))
                        {
                            found_toggle->second.on_toggle_all(value);
                        }
                        ImGui::PopStyleVar();
                        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                    }
                }
                ImGui::TableHeader(column_name.c_str());
            }

            std::vector<std::function<bool(const IFilterable&, const IFilterable&)>> sorting_functions;
            for (const auto& column : _columns)
            {
                const auto found_getter = getters.getters.find(column);
                if (found_getter != getters.getters.end())
                {
                    const auto getter = found_getter->second;
                    sorting_functions.push_back([=](const IFilterable& l, const IFilterable& r) -> bool
                        {
                            const auto l_value = getter.function(l);
                            const auto r_value = getter.function(r);
                            if (std::holds_alternative<std::string>(l_value))
                            {
                                return std::tuple(std::get<std::string>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<std::string>(r_value), r.filterable_index());
                            }
                            else if (std::holds_alternative<bool>(l_value))
                            {
                                return std::tuple(std::get<bool>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<bool>(r_value), r.filterable_index());
                            }
                            else if (std::holds_alternative<float>(l_value))
                            {
                                return std::tuple(std::get<float>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<float>(r_value), r.filterable_index());
                            }
                            else if (std::holds_alternative<int>(l_value))
                            {
                                return std::tuple(std::get<int>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<int>(r_value), r.filterable_index());
                            }
                            return l.filterable_index() < r.filterable_index();
                        });
                }
            }

            imgui_sort_weak_filters(all_items, sorting_functions, _force_sort);
            _force_sort = false;

            for (const auto& item : items)
            {
                counter.count();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                auto selected_item_ptr = selected_item.lock();
                bool selected = selected_item_ptr && selected_item_ptr == item;

                ImGuiScroller scroller;
                if (selected && _scroll_to_item)
                {
                    scroller.scroll_to_item();
                    _scroll_to_item = false;
                }

                ImGui::SetNextItemAllowOverlap();
                if (ImGui::Selectable(std::format("##{0}", item->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                {
                    scroller.fix_scroll();
                    on_item_selected(item);
                    _scroll_to_item = false;
                }
                ImGui::SameLine();

                int remaining = columns;
                for (const auto& column_name : _columns)
                {
                    const auto found_getter = getters.getters.find(column_name);
                    if (found_getter == getters.getters.end())
                    {
                        continue;
                    }
                    const auto& getter = found_getter->second;
                    const auto result = getter.function(*item);

                    if (std::holds_alternative<std::string>(result))
                    {
                        ImGui::Text(std::get<std::string>(result).c_str());
                    }
                    else if (std::holds_alternative<bool>(result))
                    {
                        bool toggle_value = std::get<bool>(result);

                        ImGui::BeginDisabled(getter.can_change == EditMode::Read);
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        if (ImGui::Checkbox(std::format("##{}-{}", column_name, item->number()).c_str(), &toggle_value))
                        {
                            auto found_toggle = on_toggle.find(column_name);
                            if (found_toggle != on_toggle.end())
                            {
                                found_toggle->second.on_toggle(item, toggle_value);
                            }
                        }
                        ImGui::PopStyleVar();
                        ImGui::EndDisabled();
                    }
                    else if (std::holds_alternative<float>(result))
                    {
                        ImGui::Text(std::to_string(std::get<float>(result)).c_str());
                    }
                    else if (std::holds_alternative<int>(result))
                    {
                        ImGui::Text(std::to_string(std::get<int>(result)).c_str());
                    }

                    if (--remaining > 0)
                    {
                        ImGui::TableNextColumn();
                    }
                }
            }

            // Track current order in case a column is removed - have to restore the order
            // manually otherwise it resets.
            _column_order = { std::from_range, ImGui::GetCurrentTable()->DisplayOrderToIndex };

            ImGui::EndTable();
            counter.render();
        }
    }
}
