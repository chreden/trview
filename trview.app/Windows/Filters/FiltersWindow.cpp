#include "FiltersWindow.h"
#include "../../Messages/Messages.h"
#include "../../Elements/ElementFilters.h"

namespace trview
{
    FiltersWindow::FiltersWindow(const std::weak_ptr<IFilterStore>& filter_store,
        const std::shared_ptr<IShell>& shell,
        const std::shared_ptr<IDialogs>& dialogs,
        const std::weak_ptr<IMessageSystem>& messaging)
        : _filter_store(filter_store), _messaging(messaging), _shell(shell), _dialogs(dialogs)
    {
        add_all_filters(_filters, {});
    }

    void FiltersWindow::initialise()
    {
        messages::get_settings(_messaging, weak_from_this());
    }

    void FiltersWindow::render()
    {
        if (!_settings)
        {
            messages::get_settings(_messaging, weak_from_this());
            return;
        }

        if (!render_filters_window())
        {
            on_window_closed();
            return;
        }
    }

    void FiltersWindow::set_number(int32_t number)
    {
        _id = std::format("Filters Manager {}", number);
    }

    void FiltersWindow::update(float)
    {
    }

    void FiltersWindow::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
        }
    }

    bool FiltersWindow::render_filters_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (auto filter_store = _filter_store.lock())
            {
                const auto filters_map = filter_store->filters();
                const auto filters = filters_map | 
                    std::views::transform([](auto&& f)
                        {
                            return f.second;
                        }) | std::ranges::to<std::vector>();
                const std::vector<std::string> keys = filters_map |
                        std::views::transform([](auto&& f) 
                            { 
                                return std::format("{} | {}", f.second.type_key, f.first); 
                            }) |
                        std::ranges::to<std::vector>();

                const std::string preview = _selected_filter < keys.size() ? keys[_selected_filter] : "";
                if (ImGui::BeginCombo("Filter", preview.c_str()))
                {
                    for (std::size_t n = 0; n < keys.size(); ++n)
                    {
                        bool is_selected = _selected_filter == n;
                        if (ImGui::Selectable(keys[n].c_str(), is_selected))
                        {
                            _selected_filter = static_cast<int32_t>(n);

                            auto found = filters[_selected_filter];
                            _filters.set_filters(found.children);
                            _filters.set_type_key(found.type_key);
                        }

                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                const auto set_filters = _filters.filters();
                if (!set_filters.empty())
                {
                    _filters.render_filters();
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    std::string FiltersWindow::type() const
    {
        return "Filters Manager";
    }

    std::string FiltersWindow::title() const
    {
        return _id;
    }
}
