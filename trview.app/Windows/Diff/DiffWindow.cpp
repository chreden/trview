#include "DiffWindow.h"
#include <format>
#include <ranges>
#include <trlevel/LevelEncryptedException.h>
#include "../../UserCancelledException.h"
#include "../../trview_imgui.h"

namespace trview
{
    namespace
    {
        constexpr ImVec4 to_colour(DiffWindow::Diff::Type type) noexcept
        {
            switch (type)
            {
            case DiffWindow::Diff::Type::Add:
                return ImVec4(0, 1, 0, 1);
            case DiffWindow::Diff::Type::Update:
                return ImVec4(1, 1, 0, 1);
            case DiffWindow::Diff::Type::Reindex:
                return ImVec4(1, 0, 1, 1);
            case DiffWindow::Diff::Type::Delete:
                return ImVec4(1, 0, 0, 1);
            }
            return ImVec4(1, 1, 1, 1);
        }

        constexpr std::string to_string(DiffWindow::Diff::Type type) noexcept
        {
            switch (type)
            {
            case DiffWindow::Diff::Type::None:
                return "None";
            case DiffWindow::Diff::Type::Add:
                return "Add";
            case DiffWindow::Diff::Type::Reindex:
                return "Reindex";
            case DiffWindow::Diff::Type::Update:
                return "Update";
            case DiffWindow::Diff::Type::Delete:
                return "Delete";
            }
            return "Unknown";
        }

        void find_direct_matches(
            std::vector<DiffWindow::Diff::Item>& results,
            const std::vector<std::shared_ptr<IItem>>& left_items,
            const std::vector<std::shared_ptr<IItem>>& right_items,
            std::vector<DiffWindow::Diff::State>& left_resolved,
            std::vector<DiffWindow::Diff::State>& right_resolved)
        {
            using Diff = DiffWindow::Diff;

            for (auto i = 0; i < results.size(); ++i)
            {
                auto& result = results[i];
                if (result.state == Diff::State::Unresolved && i < left_items.size())
                {
                    const auto left = left_items[i];
                    result.left = left;

                    if (i < right_items.size())
                    {
                        const auto right = right_items[i];
                        if (left->type_id() == right->type_id() &&
                            left->position() == right->position() &&
                            left->angle() == right->angle() &&
                            left->ocb() == right->ocb() &&
                            left->activation_flags() == right->activation_flags())
                        {
                            result.type = Diff::Type::None;
                            result.state = Diff::State::Resolved;
                            result.right = right;
                            left_resolved[i] = Diff::State::Resolved;
                            right_resolved[i] = Diff::State::Resolved;
                        }
                    }
                }
            }
        }

        void find_moves(
            std::vector<DiffWindow::Diff::Item>& results,
            const std::vector<std::shared_ptr<IItem>>& left_items,
            const std::vector<std::shared_ptr<IItem>>& right_items,
            std::vector<DiffWindow::Diff::State>& left_resolved,
            std::vector<DiffWindow::Diff::State>& right_resolved)
        {
            using Diff = DiffWindow::Diff;

            for (auto i = 0; i < results.size(); ++i)
            {
                auto& result = results[i];
                if (result.state == Diff::State::Unresolved && i < left_items.size())
                {
                    const auto left = left_items[i];
                    result.left = left;

                    // TODO: Multiple items on the same spot, deal with that.
                    const auto found =
                        std::ranges::find_if(right_items, [left, right_items, right_resolved](auto&& right)
                            {
                                return
                                    right_resolved[std::distance(right_items.begin(), std::ranges::find(right_items, right))] == Diff::State::Unresolved &&
                                    left->type_id() == right->type_id() &&
                                    left->position() == right->position() &&
                                    left->angle() == right->angle() &&
                                    left->ocb() == right->ocb() &&
                                    left->activation_flags() == right->activation_flags();
                            });

                    if (found != right_items.end())
                    {
                        result.type = Diff::Type::Reindex;
                        result.state = Diff::State::Resolved;
                        result.right = *found;
                        left_resolved[i] = Diff::State::Resolved;
                        right_resolved[std::distance(right_items.begin(), found)] = Diff::State::Resolved;
                    }
                }
            }
        }

        void mark_updated(
            std::vector<DiffWindow::Diff::Item>& results,
            const std::vector<std::shared_ptr<IItem>>& left_items,
            const std::vector<std::shared_ptr<IItem>>& right_items,
            std::vector<DiffWindow::Diff::State>& left_resolved,
            std::vector<DiffWindow::Diff::State>& right_resolved)
        {
            using Diff = DiffWindow::Diff;

            std::vector<Diff::Item> new_results;
            for (auto i = 0; i < results.size(); ++i)
            {
                auto& result = results[i];
                if (result.state == Diff::State::Unresolved)
                {
                    if (result.left.lock())
                    {
                        result.type = Diff::Type::Delete;
                        result.state = Diff::State::Resolved;
                        result.left = left_items[i];
                        left_resolved[i] = Diff::State::Resolved;
                    }
                }
            }

            for (auto i = 0; i < right_items.size(); ++i)
            {
                if (right_resolved[i] == Diff::State::Unresolved)
                {
                    // Is the left side also unresolved? Could be an update in this case.
                    if (left_resolved[i] == Diff::State::Resolved && results[i].type == Diff::Type::Delete)
                    {
                        results[i].left = left_items[i];
                        results[i].right = right_items[i];
                        results[i].type = Diff::Type::Update;
                        results[i].state = Diff::State::Resolved;
                    }
                    else
                    {
                        results.push_back(
                            {
                                .state = Diff::State::Resolved,
                                .type = Diff::Type::Add,
                                .right = right_items[i]
                            });
                    }
                    right_resolved[i] = Diff::State::Resolved;
                }
            }
        }
    }

    IDiffWindow::~IDiffWindow()
    {
    }

    DiffWindow::DiffWindow(const std::shared_ptr<IDialogs>& dialogs, const ILevel::Source& level_source)
        : _dialogs(dialogs), _level_source(level_source)
    {
    }

    void DiffWindow::render()
    {
        if (!render_diff_window())
        {
            on_window_closed();
            return;
        }
    }

    bool DiffWindow::render_diff_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open"))
                    {
                        const auto filename = _dialogs->open_file(L"Open level", { { L"All Tomb Raider Files", { L"*.tr2", L"*.tr4", L"*.trc", L"*.phd", L"*.psx" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
                        if (filename.has_value())
                        {
                            start_load(filename->filename);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (loading())
            {
                if (!_progress.empty())
                {
                    ImGui::Text(std::format("Loading: {}", _progress).c_str());
                }
            }
            else if (_diff)
            {
                render_diff_details();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void DiffWindow::set_level(const std::weak_ptr<ILevel>& level)
    {
        _level = level;
        if (_diff)
        {
            const auto left = _level.lock();
            if (left)
            {
                const auto filename = _diff->filename;
                const auto right = _diff->level;
                _load = std::async(std::launch::async, [=]() -> LoadOperation
                    {
                        LoadOperation operation
                        {
                            .level = right,
                            .filename = filename
                        };
                        operation.diff = do_diff(left, operation.level);
                        return operation;
                    });
            }
        }
    }

    void DiffWindow::set_number(int32_t number)
    {
        _id = std::format("Diff {}", number);
    }

    void DiffWindow::start_load(const std::string& filename)
    {
        const auto level = _level.lock();
        if (!level)
        {
            return;
        }

        _load = std::async(std::launch::async, [=]() -> LoadOperation
            {
                LoadOperation operation
                {
                    .filename = filename,
                };

                try
                {
                    operation.level = load_level(filename);
                }
                catch (trlevel::LevelEncryptedException&)
                {
                    operation.error = "Level is encrypted and cannot be loaded";
                }
                catch (UserCancelledException&)
                {
                }
                catch (std::exception& e)
                {
                    operation.error = std::format("Failed to load level : {}", e.what());
                }

                operation.diff = do_diff(level, operation.level);
                return operation;
            });
    }

    std::shared_ptr<ILevel> DiffWindow::load_level(const std::string& filename)
    {
        _progress = std::format("Loading {}", filename);
        auto level = _level_source(filename,
            {
                .on_progress_callback = [&](auto&& p) { _progress = p; }
            });

        level->set_filename(filename);
        return level;
    }

    DiffWindow::Diff DiffWindow::do_diff(const std::shared_ptr<ILevel>& left, const std::shared_ptr<ILevel>& right)
    {
        const auto left_items = left ->items()
            | std::views::transform([](auto&& i) { return i.lock(); })
            | std::views::filter([](auto&& i) { return i && i->ng_plus() != true; })
            | std::ranges::to<std::vector>();
        const auto right_items = right->items()
            | std::views::transform([](auto&& i) { return i.lock(); })
            | std::views::filter([](auto&& i) { return i && i->ng_plus() != true; })
            | std::ranges::to<std::vector>();

        std::vector<Diff::Item> results{ left_items.size() };
        std::vector<Diff::State> left_resolved{ left_items.size() };
        std::vector<Diff::State> right_resolved{ right_items.size() };

        find_direct_matches(results, left_items, right_items, left_resolved, right_resolved);
        find_moves(results, left_items, right_items, left_resolved, right_resolved);
        mark_updated(results, left_items, right_items, left_resolved, right_resolved);

        std::ranges::sort(
            results, [](const auto& l, const auto& r)
            {
                const auto left_left = l.left.lock();
                const auto left_right = l.right.lock();
                const auto right_left = r.left.lock();
                const auto right_right = r.right.lock();
                const auto left_index = (left_right ? left_right->number() : left_left ? left_left->number() : 0);
                const auto right_index = (right_right ? right_right->number() : right_left ? right_left->number() : 0);
                return left_index < right_index;
            }
        );

        return Diff{ .items = results };
    }

    bool DiffWindow::loading()
    {
        if (_load.valid())
        {
            if (_load.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
            {
                return true;
            }
            _diff = _load.get();
        }
        return false;
    }

    void DiffWindow::render_diff_details()
    {
        if (ImGui::BeginTabBar("TabBar"))
        {
            if (ImGui::BeginTabItem("Items"))
            {
                if (ImGui::BeginTable("Items List", 5, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY))// , ImVec2(0, -counter.height())))
                {
                    imgui_header_row(
                        {
                            { "#", _column_sizer.size(0) },
                            { "Type", _column_sizer.size(1) },
                            { "Change", _column_sizer.size(2) },
                            { "#", _column_sizer.size(3) },
                            { "Type", _column_sizer.size(4) },
                        });

                    for (const auto item_diff : _diff->diff.items)
                    {
                        const auto left_item = item_diff.left.lock();
                        const auto right_item = item_diff.right.lock();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        const std::string row_text = left_item ? std::to_string(left_item->number()) : "";
                        bool open = ImGui::TreeNodeEx(row_text.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

                        ImGui::TableNextColumn();
                        if (left_item)
                        {
                            if (ImGui::Selectable(std::format("{}##left-{}", left_item->type(), left_item->number()).c_str()))
                            {
                                on_item_selected(left_item);
                            }
                        }
                        
                        ImGui::TableNextColumn();
                        ImGui::TextColored(to_colour(item_diff.type), to_string(item_diff.type).c_str());
                        
                        ImGui::TableNextColumn();
                        if (right_item)
                        {
                            ImGui::Text(std::to_string(right_item->number()).c_str());
                        }

                        ImGui::TableNextColumn();
                        if (right_item)
                        {
                            if (ImGui::Selectable(std::format("{}##right-{}", right_item->type(), right_item->number()).c_str()))
                            {
                                on_item_selected(right_item);
                            }
                        }

                        if (open && left_item && right_item)
                        {
                            if (left_item->type_id() != right_item->type_id())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text("Type");
                                ImGui::TableNextColumn();
                                ImGui::Text(left_item->type().c_str());
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text(right_item->type().c_str());
                            }

                            if (left_item->position() != right_item->position())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text("Position");
                                ImGui::TableNextColumn();
                                ImGui::Text(std::format("{},{},{}",
                                    left_item->position().x * 1024,
                                    left_item->position().y * 1024,
                                    left_item->position().z * 1024).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text(std::format("{},{},{}",
                                    right_item->position().x * 1024,
                                    right_item->position().y * 1024,
                                    right_item->position().z * 1024).c_str());
                            }

                            if (left_item->angle() != right_item->angle())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text("Angle");
                                ImGui::TableNextColumn();
                                ImGui::Text(std::to_string(left_item->angle()).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text(std::to_string(right_item->angle()).c_str());
                            }

                            if (left_item->ocb() != right_item->ocb())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text("OCB");
                                ImGui::TableNextColumn();
                                ImGui::Text(std::to_string(left_item->ocb()).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text(std::to_string(right_item->ocb()).c_str());
                            }

                            if (left_item->activation_flags() != right_item->activation_flags())
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text("Flags");
                                ImGui::TableNextColumn();
                                ImGui::Text(format_binary(left_item->activation_flags()).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TableNextColumn();
                                ImGui::Text(format_binary(right_item->activation_flags()).c_str());
                            }

                            ImGui::TreePop();
                        }
                    }

                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    void DiffWindow::calculate_column_widths()
    {
        if (!_diff)
        {
            return;
        }

        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Type__", 1);
        _column_sizer.measure("Change__", 2);
        _column_sizer.measure("#__", 3);
        _column_sizer.measure("Type__", 4);

        for (const auto& item : _diff->diff.items)
        {
            if (auto item_ptr = item.left.lock())
            {
                _column_sizer.measure(std::to_string(item_ptr->number()), 0);
                _column_sizer.measure(item_ptr->type(), 1);
            }

            _column_sizer.measure(to_string(item.type), 2);

            if (auto item_ptr = item.right.lock())
            {
                _column_sizer.measure(std::to_string(item_ptr->number()), 3);
                _column_sizer.measure(item_ptr->type(), 4);
            }
        }
    }
}
