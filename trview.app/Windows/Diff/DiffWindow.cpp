#include "DiffWindow.h"
#include <format>
#include <ranges>
#include <trlevel/LevelEncryptedException.h>
#include "../../UserCancelledException.h"
#include "../../trview_imgui.h"

#include "../../Elements/ITrigger.h"
#include "../../Elements/ILight.h"
#include "../../Elements/SoundSource/ISoundSource.h"
#include "../../Elements/IRoom.h"
#include "../../Elements/ISector.h"

namespace trview
{
    namespace
    {
        using std::to_string;

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

        std::string to_string(const DirectX::SimpleMath::Vector3& value)
        {
            return std::format("{},{},{}", value.x, value.y, value.z);
        }

        std::string to_string(const Colour& colour)
        {
            return std::format("{},{},{},{}", colour.r, colour.g, colour.b, colour.a);
        }

        template <typename T>
        std::string to_string(const std::optional<T>& value)
        {
            return value.has_value() ? to_string(*value) : "";
        }

        bool equal_float(float l, float r)
        {
            if (std::isnan(l) && std::isnan(r))
            {
                return true;
            }
            return l == r;
        }

        template <typename T>
        struct Entry
        {
            DiffWindow::Diff::State state;
            std::shared_ptr<T>      entry;
        };

        template <typename T>
        void find_direct_matches(
            std::vector<DiffWindow::Diff::Item<T>>& results,
            std::vector<Entry<T>>& left_entries,
            std::vector<Entry<T>>& right_entries,
            std::function<bool (const std::shared_ptr<T>&, const std::shared_ptr<T>&)> comparer)
        {
            using Diff = DiffWindow::Diff;

            for (auto i = 0; i < results.size(); ++i)
            {
                auto& result = results[i];
                if (result.state == Diff::State::Unresolved && i < left_entries.size())
                {
                    auto& left = left_entries[i];
                    result.left = left.entry;

                    if (i < right_entries.size())
                    {
                        auto& right = right_entries[i];
                        if (comparer(left.entry, right.entry))
                        {
                            result.type = Diff::Type::None;
                            result.state = Diff::State::Resolved;
                            result.right = right.entry;
                            left.state = Diff::State::Resolved;
                            right.state = Diff::State::Resolved;
                        }
                    }
                }
            }
        }

        template <typename T>
        void find_moves(
            std::vector<DiffWindow::Diff::Item<T>>& results,
            std::vector<Entry<T>>& left_entries,
            std::vector<Entry<T>>& right_entries,
            std::function<bool (const std::shared_ptr<T>&, const std::shared_ptr<T>&)> comparer)
        {
            using Diff = DiffWindow::Diff;

            for (auto i = 0; i < results.size(); ++i)
            {
                auto& result = results[i];
                if (result.state == Diff::State::Unresolved && i < left_entries.size())
                {
                    auto& left = left_entries[i];
                    result.left = left.entry;

                    // TODO: Multiple items on the same spot, deal with that.
                    auto found =
                        std::ranges::find_if(right_entries, [left, right_entries, comparer](auto&& right)
                            {
                                return right.state == Diff::State::Unresolved && comparer(left.entry, right.entry);
                            });

                    if (found != right_entries.end())
                    {
                        result.type = Diff::Type::Reindex;
                        result.state = Diff::State::Resolved;
                        result.right = found->entry;
                        left.state = Diff::State::Resolved;
                        found->state = Diff::State::Resolved;
                    }
                }
            }
        }

        template <typename T>
        void mark_updated(
            std::vector<DiffWindow::Diff::Item<T>>& results,
            std::vector<Entry<T>>& left_entries,
            std::vector<Entry<T>>& right_entries)
        {
            using Diff = DiffWindow::Diff;

            std::vector<Diff::Item<T>> new_results;
            for (auto i = 0; i < results.size(); ++i)
            {
                auto& result = results[i];
                if (result.state == Diff::State::Unresolved)
                {
                    if (result.left.lock())
                    {
                        result.type = Diff::Type::Delete;
                        result.state = Diff::State::Resolved;
                        result.left = left_entries[i].entry;
                        left_entries[i].state = Diff::State::Resolved;
                    }
                }
            }

            for (auto i = 0; i < right_entries.size(); ++i)
            {
                if (right_entries[i].state == Diff::State::Unresolved)
                {
                    // Is the left side also unresolved? Could be an update in this case.
                    if (i < left_entries.size() && left_entries[i].state == Diff::State::Resolved && results[i].type == Diff::Type::Delete)
                    {
                        results[i].left = left_entries[i].entry;
                        results[i].right = right_entries[i].entry;
                        results[i].type = Diff::Type::Update;
                        results[i].state = Diff::State::Resolved;
                    }
                    else
                    {
                        results.push_back(
                            {
                                .state = Diff::State::Resolved,
                                .type = Diff::Type::Add,
                                .right = right_entries[i].entry
                            });
                    }
                    right_entries[i].state = Diff::State::Resolved;
                }
            }
        }

        struct DiffDetail
        {
            std::string type;
            std::string left;
            std::string right;
        };

        void add_if_changed(auto&& results, auto&& name, auto&& left, auto&& right, auto&& left_text, auto&& right_text)
        {
            if (left != right)
            {
                results.push_back({ .type = name, .left = left_text, .right = right_text });
            }
        }

        void add_if_changed(auto&& results, auto&& name, auto&& left, auto&& right)
        {
            if constexpr (std::is_same<std::remove_reference_t<decltype(left)>, std::string>::value)
            {
                add_if_changed(results, name, left, right, left, right);
            }
            else
            {
                add_if_changed(results, name, left, right, to_string(left), to_string(right));
            }
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<IItem>& left, const std::shared_ptr<IItem>& right)
        {
            std::vector<DiffDetail> results;
            add_if_changed(results, "Type", left->type_id(), right->type_id(), left->type(), right->type());
            add_if_changed(results, "Position", left->position() * 1024, right->position() * 1024);
            add_if_changed(results, "Angle", left->angle(), right->angle());
            add_if_changed(results, "OCB", left->ocb(), right->ocb());
            add_if_changed(results, "Flags", format_binary(left->activation_flags()), format_binary(right->activation_flags()));
            return results;
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<ITrigger>& left, const std::shared_ptr<ITrigger>& right)
        {
            std::vector<DiffDetail> results;
            add_if_changed(results, "Type", left->type(), right->type());
            add_if_changed(results, "Only Once", left->only_once(), right->only_once());
            add_if_changed(results, "Position", left->position() * 1024, right->position() * 1024);
            add_if_changed(results, "Flags", format_binary(left->flags()), format_binary(right->flags()));
            add_if_changed(results, "Timer", left->timer(), right->timer());
            return results;
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<ILight>& left, const std::shared_ptr<ILight>& right)
        {
            std::vector<DiffDetail> results;
            add_if_changed(results, "Type", left->type(), right->type());
            add_if_changed(results, "Position", left->position() * 1024, right->position() * 1024);
            add_if_changed(results, "Colour", left->colour(), right->colour());
            add_if_changed(results, "Intensity", left->intensity(), right->intensity());
            add_if_changed(results, "Fade", left->fade(), right->fade());
            add_if_changed(results, "Direction", left->direction() * 1024, right->direction() * 1024);
            add_if_changed(results, "In", left->in(), right->in());
            add_if_changed(results, "Out", left->out(), right->out());
            add_if_changed(results, "Rad In", left->rad_in(), right->rad_in());
            add_if_changed(results, "Rad Out", left->rad_out(), right->rad_out());
            add_if_changed(results, "Range", left->range(), right->range());
            add_if_changed(results, "Length", left->length(), right->length());
            add_if_changed(results, "Cutoff", left->cutoff(), right->cutoff());
            add_if_changed(results, "Radius", left->radius(), right->radius());
            add_if_changed(results, "Density", left->density(), right->density());
            return results;
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<ICameraSink>& left, const std::shared_ptr<ICameraSink>& right)
        {
            std::vector<DiffDetail> results;
            add_if_changed(results, "Type", left->type(), right->type());
            add_if_changed(results, "Position", left->position() * 1024, right->position() * 1024);
            add_if_changed(results, "Box Index", left->box_index(), right->box_index());
            add_if_changed(results, "Flag", left->flag(), right->flag());
            add_if_changed(results, "Persistent", left->persistent(), right->persistent());
            add_if_changed(results, "Strength", left->strength(), right->strength());
            return results;
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<IStaticMesh>& left, const std::shared_ptr<IStaticMesh>& right)
        {
            std::vector<DiffDetail> results;
            add_if_changed(results, "Type", left->type(), right->type());
            add_if_changed(results, "Position", left->position() * 1024, right->position() * 1024);
            add_if_changed(results, "Rotation", left->rotation(), right->rotation());
            add_if_changed(results, "ID", left->id(), right->id());
            add_if_changed(results, "Flags", left->flags(), right->flags(), format_binary(left->flags()), format_binary(right->flags()));
            add_if_changed(results, "Breakable", left->breakable(), right->breakable());
            add_if_changed(results, "Has Collision", left->has_collision(), right->has_collision());
            return results;
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<ISoundSource>& left, const std::shared_ptr<ISoundSource>& right)
        {
            std::vector<DiffDetail> results;
            add_if_changed(results, "Position", left->position() * 1024, right->position() * 1024);
            add_if_changed(results, "ID", left->id(), right->id());
            add_if_changed(results, "Flags", left->flags(), right->flags(), format_binary(left->flags()), format_binary(right->flags()));
            add_if_changed(results, "Chance", left->chance(), right->chance());
            add_if_changed(results, "Characteristics", left->characteristics(), right->characteristics());
            add_if_changed(results, "Pitch", left->pitch(), right->pitch());
            add_if_changed(results, "Range", left->range(), right->range());
            add_if_changed(results, "Volume", left->volume(), right->volume());
            add_if_changed(results, "Sample", left->sample(), right->sample());
            return results;
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<IRoom>& left, const std::shared_ptr<IRoom>& right)
        {
            std::vector<DiffDetail> results;
            // left->bounding_box() == right->bounding_box() &&
            // info
            add_if_changed(results, "Alternate Group", left->alternate_group(), right->alternate_group());
            add_if_changed(results, "Alternate Mode", left->alternate_mode(), right->alternate_mode());
            add_if_changed(results, "Ambient", left->ambient(), right->ambient());
            add_if_changed(results, "Ambient Intensity 1", left->ambient_intensity_1(), right->ambient_intensity_1());
            add_if_changed(results, "Ambient Intensity 2", left->ambient_intensity_2(), right->ambient_intensity_2());
            add_if_changed(results, "Centre", left->centre() * 1024, right->centre() * 1024);
            add_if_changed(results, "Flags", left->flags(), right->flags());
            add_if_changed(results, "Light Mode", left->light_mode(), right->light_mode());
            add_if_changed(results, "X Sectors", left->num_x_sectors(), right->num_x_sectors());
            add_if_changed(results, "Z Sectors", left->num_z_sectors(), right->num_z_sectors());
            return results;
        }

        std::vector<DiffDetail> get_details(const std::shared_ptr<ISector>& left, const std::shared_ptr<ISector>& right)
        {
            std::vector<DiffDetail> results;
            add_if_changed(results, "X", left->x(), right->x());
            add_if_changed(results, "Z", left->z(), right->z());
            add_if_changed(results, "Flags", static_cast<int>(left->flags()), static_cast<int>(right->flags()));
            add_if_changed(results, "Corner 0", left->corners()[0], right->corners()[0]);
            add_if_changed(results, "Corner 1", left->corners()[1], right->corners()[1]);
            add_if_changed(results, "Corner 2", left->corners()[2], right->corners()[2]);
            add_if_changed(results, "Corner 3", left->corners()[3], right->corners()[3]);
            add_if_changed(results, "Ceiling Corner 0", left->ceiling_corners()[0], right->ceiling_corners()[0]);
            add_if_changed(results, "Ceiling Corner 1", left->ceiling_corners()[1], right->ceiling_corners()[1]);
            add_if_changed(results, "Ceiling Corner 2", left->ceiling_corners()[2], right->ceiling_corners()[2]);
            add_if_changed(results, "Ceiling Corner 3", left->ceiling_corners()[3], right->ceiling_corners()[3]);
            add_if_changed(results, "Tilt X", left->tilt_x(), right->tilt_x());
            add_if_changed(results, "Tilt Z", left->tilt_z(), right->tilt_z());
            return results;
        }

        template <typename T>
        void show_details(const std::shared_ptr<T>& left, const std::shared_ptr<T>& right)
        {
            for (const auto& detail : get_details(left, right))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::Text(detail.type.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(detail.left.c_str());
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                ImGui::Text(detail.right.c_str());
            }
        }

        template <typename T>
        auto get_results(auto&& left_entries, auto&& right_entries, auto&& comparer, auto&& extra_check)
        {
            auto left_filtered = left_entries
                | std::views::transform([](auto&& t) { return t.lock(); })
                | std::views::filter([](auto&& t) { return t != nullptr; })
                | std::views::filter(extra_check)
                | std::views::transform([](auto&& t) -> Entry<T> { return { .state = DiffWindow::Diff::State::Unresolved, .entry = t }; })
                | std::ranges::to<std::vector>();
            auto right_filtered = right_entries
                | std::views::transform([](auto&& t) { return t.lock(); })
                | std::views::filter([](auto&& t) { return t != nullptr; })
                | std::views::filter(extra_check)
                | std::views::transform([](auto&& t) -> Entry<T> { return { .state = DiffWindow::Diff::State::Unresolved, .entry = t }; })
                | std::ranges::to<std::vector>();

            std::vector<DiffWindow::Diff::Item<T>> results{ left_filtered.size() };

            find_direct_matches<T>(results, left_filtered, right_filtered, comparer);
            find_moves<T>(results, left_filtered, right_filtered, comparer);
            mark_updated<T>(results, left_filtered, right_filtered);

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

            return results;
        };

        template <typename T>
        auto get_results(auto&& left_entries, auto&& right_entries, auto&& comparer)
        {
            return get_results<T>(left_entries, right_entries, comparer, [](auto&&) { return true; });
        };
    }

    IDiffWindow::~IDiffWindow()
    {
    }

    DiffWindow::DiffWindow(const std::shared_ptr<IDialogs>& dialogs, const ILevel::Source& level_source, std::unique_ptr<IFileMenu> file_menu)
        : _dialogs(dialogs), _level_source(level_source), _file_menu(std::move(file_menu))
    {
        _token_store += _file_menu->on_file_open += [this](auto&& filename) { start_load(filename); };
    }

    void DiffWindow::render()
    {
        if (!render_diff_window())
        {
            if (_diff.has_value())
            {
                on_diff_ended(_diff->level);
            }
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
            if (!_load.valid() && ImGui::BeginMenuBar())
            {
                _file_menu->render();
                ImGui::MenuItem("Only Show Changes", nullptr, &_only_show_changes);
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
        _diff.reset();
    }

    void DiffWindow::set_number(int32_t number)
    {
        _id = std::format("Diff {}", number);
    }

    void DiffWindow::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        _file_menu->set_recent_files(_settings.recent_diff_files);
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
                    operation.diff = do_diff(level, operation.level);
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
        // Items:
        _progress = "Comparing items...";
        const auto item_results = get_results<IItem>(left->items(), right->items(),
            [](auto&& left, auto&& right)
            {
                return left->type_id() == right->type_id() &&
                    left->position() == right->position() &&
                    left->angle() == right->angle() &&
                    left->ocb() == right->ocb() &&
                    left->activation_flags() == right->activation_flags();
            },
            [](auto&& i) { return i && i->ng_plus() != true; });

        // Triggers:
        _progress = "Comparing triggers...";
        const auto trigger_results = get_results<ITrigger>(left->triggers(), right->triggers(), 
            [](auto&& left, auto&& right)
            {
                return
                    // room?
                    // x/z
                    left->type() == right->type() &&
                    left->only_once() == right->only_once() &&
                    left->flags() == right->flags() &&
                    left->timer() == right->timer() &&
                    left->position() == right->position();
            });;

        // Lights:
        _progress = "Comparing lights...";
        const auto light_results = get_results<ILight>(left->lights(), right->lights(),
            [](auto&& left, auto&& right)
            {
                return
                    left->type() == right->type() &&
                    left->position() == right->position() &&
                    left->colour() == right->colour() &&
                    left->intensity() == right->intensity() &&
                    left->fade() == right->fade() &&
                    left->direction() == right->direction() &&
                    equal_float(left->in(), right->in()) &&
                    equal_float(left->out(), right->out()) &&
                    equal_float(left->rad_in(), right->rad_in()) &&
                    equal_float(left->rad_out(), right->rad_out()) &&
                    equal_float(left->range(), right->range()) &&
                    equal_float(left->length(), right->length()) &&
                    equal_float(left->cutoff(), right->cutoff()) &&
                    equal_float(left->radius(), right->radius()) &&
                    equal_float(left->density(), right->density());
            });

        // Camera/Sinks:
        _progress = "Comparing camera/sinks...";
        const auto camera_sink_results = get_results<ICameraSink>(left->camera_sinks(), right->camera_sinks(),
            [](auto&& left, auto&& right)
            {
                return
                    left->type() == right->type() &&
                    left->position() == right->position() &&
                    left->box_index() == right->box_index() &&
                    left->flag() == right->flag() &&
                    left->persistent() == right->persistent() &&
                    left->strength() == right->strength();
                // bounding_box
            });

        // Statics:
        _progress = "Comparing statics...";
        const auto static_results = get_results<IStaticMesh>(left->static_meshes(), right->static_meshes(),
            [](auto&& left, auto&& right)
            {
                return
                    // Room?
                    // visibility
                    // collision
                    left->type() == right->type() &&
                    left->position() == right->position() &&
                    left->rotation() == right->rotation() &&
                    left->id() == right->id() &&
                    left->flags() == right->flags() &&
                    left->breakable() == right->breakable() &&
                    left->has_collision() == right->has_collision();
            });

        // Sounds:
        _progress = "Comparing sounds...";
        const auto sound_source_results = get_results<ISoundSource>(left->sound_sources(), right->sound_sources(),
            [](auto&& left, auto&& right)
            {
                return
                    left->chance() == right->chance() &&
                    left->characteristics() == right->characteristics() &&
                    left->flags() == right->flags() &&
                    left->id() == right->id() &&
                    left->position() == right->position() &&
                    left->pitch() == right->pitch() &&
                    left->range() == right->range() &&
                    left->volume() == right->volume() &&
                    left->sample() == right->sample();
            });

        // Rooms:
        _progress = "Comparing rooms...";
        const auto rooms_results = get_results<IRoom>(left->rooms(), right->rooms(),
            [](auto&& left, auto&& right)
            {
                return
                    left->alternate_group() == right->alternate_group() &&
                    left->ambient() == right->ambient() &&
                    left->ambient_intensity_1() == right->ambient_intensity_1() &&
                    left->ambient_intensity_2() == right->ambient_intensity_2() &&
                    left->alternate_mode() == right->alternate_mode() &&
                    // left->bounding_box() == right->bounding_box() &&
                    left->centre() == right->centre() &&
                    left->flags() == right->flags() &&
                    // info
                    left->light_mode() == right->light_mode()&&
                    left->num_x_sectors() == right->num_x_sectors() &&
                    left->num_z_sectors() == right->num_z_sectors();
            });

        // Sectors:
        _progress = "Comparing sectors...";
        const auto left_sectors = std::ranges::join_view(
                left->rooms() |
                std::views::transform([](auto&& r) { return r.lock(); }) |
                std::views::transform([](auto&& r) { return r->sectors(); }) |
                std::ranges::to<std::vector>()) |
            std::views::transform([](auto&& s) -> std::weak_ptr<ISector> { return s; }) |
            std::ranges::to<std::vector>();
        const auto right_sectors = std::ranges::join_view(
                right->rooms() |
                std::views::transform([](auto&& r) { return r.lock(); }) |
                std::views::transform([](auto&& r) { return r->sectors(); }) |
                std::ranges::to<std::vector>()) |
            std::views::transform([](auto&& s) -> std::weak_ptr<ISector> { return s; }) |
            std::ranges::to<std::vector>();

        const auto sectors_results = get_results<ISector>(left_sectors, right_sectors,
            [](auto&& left, auto&& right)
            {
                return
                    left->x() == right->x() &&
                    left->z() == right->z() &&
                    // left->floordata_index() == right->floordata_index() &&
                    left->flags() == right->flags() &&
                    left->corners() == right->corners() &&
                    left->ceiling_corners() == right->ceiling_corners() &&
                    left->tilt_x() == right->tilt_x() &&
                    left->tilt_z() == right->tilt_z();
            }, [](auto&&) { return true; });

        return Diff
        {
            .items = item_results,
            .triggers = trigger_results,
            .lights = light_results,
            .camera_sinks = camera_sink_results,
            .static_meshes = static_results,
            .sound_sources = sound_source_results,
            .rooms = rooms_results,
            .sectors = sectors_results
        };
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
            if (_diff->level)
            {
                _file_menu->open_file(_diff->level->filename());
                _settings.add_recent_diff_file(_diff->level->filename());
                _file_menu->set_recent_files(_settings.recent_diff_files);
                on_settings(_settings);
            }
        }
        return false;
    }

    void DiffWindow::render_diff_details()
    {
        std::string a_filename;
        if (auto level = _level.lock())
        {
            a_filename = level->filename();
        }
        ImGui::Text(std::format("A: {}", a_filename).c_str());
        ImGui::Text(std::format("B: {}", _diff->filename).c_str());

        if (ImGui::BeginTabBar("TabBar"))
        {
            const auto any_diff = [](auto&& range) { return std::ranges::any_of(range, [](auto&& e) { return e.type != Diff::Type::None; }); };
            const bool any_items = any_diff(_diff->diff.items);
            const bool any_triggers = any_diff(_diff->diff.triggers);
            const bool any_lights = any_diff(_diff->diff.lights);
            const bool any_camera_sink = any_diff(_diff->diff.camera_sinks);
            const bool any_statics = any_diff(_diff->diff.static_meshes);
            const bool any_sounds = any_diff(_diff->diff.sound_sources);
            const bool any_rooms = any_diff(_diff->diff.rooms);
            const bool any_sectors = any_diff(_diff->diff.sectors);

            const auto show_table = [this](
                const std::string& table_name,
                const auto& diff_entries,
                auto& on_selected,
                const auto& key_get)
                {
                    if (ImGui::BeginTable(table_name.c_str(), 5, ImGuiTableFlags_ScrollY))
                    {
                        imgui_header_row(
                            {
                                { "# ##L", _column_sizer.size(0) },
                                { "Type##L", _column_sizer.size(1) },
                                { "Change", _column_sizer.size(2) },
                                { "# ##R", _column_sizer.size(3) },
                                { "Type##R", _column_sizer.size(4) },
                            });

                        for (const auto item_diff : diff_entries)
                        {
                            if (_only_show_changes && item_diff.type == Diff::Type::None)
                            {
                                continue;
                            }

                            const auto left_item = item_diff.left.lock();
                            const auto right_item = item_diff.right.lock();

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();

                            const std::string row_text = left_item ? std::to_string(left_item->number()) : "";
                            bool open = false;
                            if (item_diff.type == Diff::Type::Update)
                            {
                                open = ImGui::TreeNodeEx(row_text.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
                            }
                            else
                            {
                                ImGui::Text(row_text.c_str());
                            }

                            ImGui::TableNextColumn();
                            if (left_item)
                            {
                                if (ImGui::Selectable(std::format("{}##left-{}", key_get(left_item), left_item->number()).c_str()))
                                {
                                    on_selected(left_item);
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
                                if (ImGui::Selectable(std::format("{}##right-{}", key_get(right_item), right_item->number()).c_str()))
                                {
                                    on_selected(right_item);
                                }
                            }

                            if (open && left_item && right_item)
                            {
                                show_details(left_item, right_item);
                                ImGui::TreePop();
                            }
                        }

                        ImGui::EndTable();
                    }
                };
            
            if (ImGui::BeginTabItem(std::format("Items{}", any_items ? "*" : "").c_str()))
            {
                show_table("Items List", _diff->diff.items, on_item_selected, [](auto&& i) { return i->type(); });
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(std::format("Triggers{}", any_triggers ? "*" : "").c_str()))
            {
                show_table("Triggers List", _diff->diff.triggers, on_trigger_selected, [](auto&& t) { return to_string(t->type()); });
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(std::format("Lights{}", any_lights ? "*" : "").c_str()))
            {
                show_table("Lights List", _diff->diff.lights, on_light_selected, [](auto&& l) { return to_string(l->type()); });
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(std::format("Camera/Sink{}", any_camera_sink ? "*" : "").c_str()))
            {
                show_table("CameraSink List", _diff->diff.camera_sinks, on_camera_sink_selected, [](auto&& c) { return to_string(c->type()); });
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(std::format("Statics{}", any_statics ? "*" : "").c_str()))
            {
                show_table("Statics List", _diff->diff.static_meshes, on_static_mesh_selected, [](auto&& c) { return to_string(c->type()); });
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(std::format("Sound Sources{}", any_sounds ? "*" : "").c_str()))
            {
                show_table("Sounds List", _diff->diff.sound_sources, on_sound_source_selected, [](auto&&) { return "Sound"; });
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(std::format("Rooms{}", any_rooms ? "*" : "").c_str()))
            {
                show_table("Rooms List", _diff->diff.rooms, on_room_selected, [](auto&&) { return "Room"; });
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(std::format("Sectors{}", any_sectors ? "*" : "").c_str()))
            {
                show_table("Sectors List", _diff->diff.sectors, on_sector_selected, [](auto&&) { return "Sector"; });
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
