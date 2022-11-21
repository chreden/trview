#pragma once

namespace trview
{
    template < typename T >
    void imgui_sort(std::vector<T>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks, bool force_sort)
    {
        auto specs = ImGui::TableGetSortSpecs();
        if (specs && (specs->SpecsDirty || force_sort))
        {
            std::sort(container.begin(), container.end(),
                [&](const auto& l, const auto& r) -> int
                {
                    const auto& callback = callbacks[specs->Specs[0].ColumnIndex];
                    const auto result = callback(l, r);
                    return specs->Specs->SortDirection == ImGuiSortDirection_Ascending ? result : !result;
                });
            specs->SpecsDirty = false;
        }
    }

    template < typename T >
    void imgui_sort_weak(std::vector<std::weak_ptr<T>>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks, bool force_sort)
    {
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
}
