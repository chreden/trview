#include "ItemsWindow.h"
#include <trview.app/Resources/resource.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

using namespace trview::graphics;

namespace trview
{
    namespace
    {
        const float Height{ 420 };

        ui::Listbox::Item create_listbox_item(const Item& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"ID", std::to_wstring(item.type_id()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", item.type() },
                     { L"Hide", std::to_wstring(!item.visible()) }} };
        }
    }

    const std::string ItemsWindow::Names::add_to_route_button{ "AddToRoute" };
    const std::string ItemsWindow::Names::items_listbox{ "Items" };
    const std::string ItemsWindow::Names::stats_listbox{ "Stats" };
    const std::string ItemsWindow::Names::sync_item_checkbox{ "SyncItem" };
    const std::string ItemsWindow::Names::track_room_checkbox{ "TrackRoom" };
    const std::string ItemsWindow::Names::triggers_listbox{ "Triggers" };
    const std::string ItemsWindow::Names::expander{ "Expander" };

    ItemsWindow::ItemsWindow(const IDeviceWindow::Source& device_window_source, const ui::render::IRenderer::Source& renderer_source, const ui::IInput::Source& input_source, const Window& parent,
        const std::shared_ptr<IClipboard>& clipboard, const IBubble::Source& bubble_source, const std::shared_ptr<ui::ILoader>& ui_source)
        : CollapsiblePanel(device_window_source, renderer_source(Size(450, Height)), parent, L"trview.items", L"Items", input_source, Size(450, Height), ui_source->load_from_resource(IDR_UI_ITEMS_WINDOW)),
        _clipboard(clipboard), _bubble(bubble_source(*_ui)), _tooltip(std::make_unique<Tooltip>(*_ui))
    {
        _tips[L"OCB"] = L"Changes entity behaviour";
        _tips[L"Clear Body"] = L"Removed when Bodybag is triggered";

        CollapsiblePanel::on_window_closed += IItemsWindow::on_window_closed;
        bind_controls();
    }

    void ItemsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        for (const auto& i : _all_items)
        {
            _selected[i.number()] = false;
        }
        populate_items(items);
    }

    void ItemsWindow::update_items(const std::vector<Item>& items)
    {
        set_items(items);
        if (_track_room)
        {
            _filter_applied = false;
            set_current_room(_current_room);
        }
    }

    void ItemsWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
        _trigger_list->set_items({});
    }

    void ItemsWindow::clear_selected_item()
    {
        _selected_item.reset();
        _stats_list->set_items({});
        _trigger_list->set_items({});
    }

    void ItemsWindow::populate_items(const std::vector<Item>& items)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(items.begin(), items.end(), std::back_inserter(list_items), create_listbox_item);
        _items_list->set_items(list_items);
    }

    void ItemsWindow::set_current_room(uint32_t room)
    {
        if (_track_room && (!_filter_applied || _current_room != room))
        {
            _filter_applied = true;

            std::vector<Item> filtered_items;
            for (const auto& item : _all_items)
            {
                if (item.room() == room)
                {
                    filtered_items.push_back(item);
                }
            }
            populate_items(filtered_items);
        }

        _current_room = room;
    }

    void ItemsWindow::bind_controls()
    {
        using namespace ui;

        _track_room_checkbox = _ui->find<Checkbox>(Names::track_room_checkbox);
        _token_store += _track_room_checkbox->on_state_changed += [this](bool value)
        {
            set_track_room(value);
        };

        auto sync_item = _ui->find<Checkbox>(Names::sync_item_checkbox);
        sync_item->set_state(_sync_item);
        _token_store += sync_item->on_state_changed += [this](bool value) { set_sync_item(value); };

        set_expander(_ui->find<Button>(Names::expander));

        _items_list = _ui->find<Listbox>(Names::items_listbox);
        _token_store += _items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_item_details(_all_items[index]);
            if (_sync_item)
            {
                on_item_selected(_all_items[index]);
            }
        };
        _token_store += _items_list->on_state_changed += [&](const auto& item, const std::wstring& column, bool state)
        {
            if (column == L"Hide")
            {
                auto index = std::stoi(item.value(L"#"));
                on_item_visibility(_all_items[index], !state);
            }
        };

        _stats_list = _ui->find<Listbox>(Names::stats_listbox);
        _token_store += _stats_list->on_item_selected += [this](const ui::Listbox::Item& item)
        {
            _clipboard->write(window(), item.value(L"Value"));
            _bubble->show(client_cursor_position(window()) - Point(0, 20));
        };

        auto add_to_route = _ui->find<Button>(Names::add_to_route_button);
        _token_store += add_to_route->on_click += [&]()
        {
            if (_selected_item.has_value())
            {
                on_add_to_route(_selected_item.value());
            }
        };

        _trigger_list = _ui->find<Listbox>(Names::triggers_listbox);
        _token_store += _trigger_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            set_track_room(false);
            on_trigger_selected(_all_triggers[index]);
        };
    }

    void ItemsWindow::load_item_details(const Item& item)
    {
        auto make_item = [](const auto& name, const auto& value)
        {
            return Listbox::Item { { { L"Name", name }, { L"Value", value } } };
        };

        auto position_text = [&item]()
        {
            std::wstringstream stream;
            stream << std::fixed << std::setprecision(0) <<
                item.position().x * trlevel::Scale_X << L", " <<
                item.position().y * trlevel::Scale_Y << L", " <<
                item.position().z * trlevel::Scale_Z;
            return stream.str();
        };

        using namespace ui;
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", item.type()));
        stats.push_back(make_item(L"#", std::to_wstring(item.number())));
        stats.push_back(make_item(L"Position", position_text()));
        stats.push_back(make_item(L"Type ID", std::to_wstring(item.type_id())));
        stats.push_back(make_item(L"Room", std::to_wstring(item.room())));
        stats.push_back(make_item(L"Clear Body", format_bool(item.clear_body_flag())));
        stats.push_back(make_item(L"Invisible", format_bool(item.invisible_flag())));
        stats.push_back(make_item(L"Flags", format_binary(item.activation_flags())));
        stats.push_back(make_item(L"OCB", std::to_wstring(item.ocb())));
        _stats_list->set_items(stats);

        bind_tooltip();

        std::vector<Listbox::Item> triggers;
        for (auto& trigger : item.triggers())
        {
            auto trigger_ptr = trigger.lock();
            if (!trigger_ptr)
            {
                continue;
            }
            triggers.push_back(
                {
                    {
                        { L"#", std::to_wstring(trigger_ptr->number()) },
                        { L"Room", std::to_wstring(trigger_ptr->room()) },
                        { L"Type", trigger_type_name(trigger_ptr->type()) },
                    }
                });
        }
        _trigger_list->set_items(triggers);
    }

    void ItemsWindow::set_track_room(bool value)
    {
        if (_track_room != value)
        {
            _track_room = value;
            if (_track_room)
            {
                set_current_room(_current_room);
            }
            else
            {
                _filter_applied = false;
                set_items(_all_items);
            }
        }

        if (_track_room_checkbox->state() != _track_room)
        {
            _track_room_checkbox->set_state(_track_room);
        }
    }

    void ItemsWindow::set_sync_item(bool value)
    {
        if (_sync_item != value)
        {
            _sync_item = value;
            if (_selected_item.has_value())
            {
                set_selected_item(_selected_item.value());
            }
        }
    }

    void ItemsWindow::set_selected_item(const Item& item)
    {
        _selected_item = item;
        if (_sync_item)
        {
            const auto& list_item = create_listbox_item(item);
            if (_items_list->set_selected_item(list_item))
            {
                load_item_details(item);
            }
            else
            {
                _selected_item.reset();
            }
        }
    }

    std::optional<Item> ItemsWindow::selected_item() const
    {
        return _selected_item;
    }

    void ItemsWindow::render_host()
    {
        ImGui::Begin("Items", nullptr, ImGuiWindowFlags_NoDocking);
        ImGuiID dockspaceID = ImGui::GetID("dockspace");
        if (!ImGui::DockBuilderGetNode(dockspaceID))
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::DockBuilderRemoveNode(dockspaceID);
            ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar);
            ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

            ImGuiID dock_main_id = dockspaceID;
            ImGui::DockBuilderDockWindow("ItemsList", dock_main_id);
            ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, NULL, &dock_main_id);
            ImGui::DockBuilderDockWindow("ItemsDetails", dock_id_right);

            ImGui::DockBuilderGetNode(dock_main_id)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
            ImGui::DockBuilderGetNode(dock_id_right)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

            ImGui::DockBuilderFinish(dockspaceID);
        }
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);
        ImGui::End();
    }

    void ItemsWindow::render(bool vsync)
    {
        CollapsiblePanel::render(vsync);

        render_host();

        // TODO: Unique ID.
        if (ImGui::Begin("ItemsList", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            bool track_room = _track_room;
            if (ImGui::Checkbox("Track Room##trackroom", &track_room))
            {
                set_track_room(track_room);
            }
            ImGui::SameLine();
            bool sync_item = _sync_item;
            if (ImGui::Checkbox("Sync Item##syncitem", &sync_item))
            {
                set_sync_item(sync_item);
            }

            if (ImGui::BeginTable("##itemslist", 5, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Hide");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                for (const auto& item : _all_items)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    // ImGui::Selectable((std::to_string(item.number()) + std::string("##") + std::to_string(item.number())).c_str(), &_selected[item.number()], ImGuiSelectableFlags_SpanAllColumns);
                    ImGui::Text(std::to_string(item.number()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item.room()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item.type_id()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(to_utf8(item.type()).c_str());
                    ImGui::TableNextColumn();
                    bool x = !item.visible();
                    if (ImGui::Checkbox((std::string("##hide-") + std::to_string(item.number())).c_str(), &x))
                    {
                        on_item_visibility(item, !x);
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();

        if (ImGui::Begin("ItemsDetails", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Text("Item Details");
            if (ImGui::BeginTable("##itemstats", 2, 0, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                if (_selected_item.has_value())
                {
                    const auto& item = _selected_item.value();

                    auto add_stat = [&item](const std::string& name, const std::string& value)
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text(name.c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(value.c_str());
                    };

                    auto position_text = [&item]()
                    {
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(0) <<
                            item.position().x * trlevel::Scale_X << ", " <<
                            item.position().y * trlevel::Scale_Y << ", " <<
                            item.position().z * trlevel::Scale_Z;
                        return stream.str();
                    };

                    add_stat("Type", to_utf8(item.type()));
                    add_stat("#", std::to_string(item.number()));
                    add_stat("Position", position_text());
                    add_stat("Type ID", std::to_string(item.type_id()));
                    add_stat("Room", std::to_string(item.room()));
                    add_stat("Clear Body", to_utf8(format_bool(item.clear_body_flag())));
                    add_stat("Invisible", to_utf8(format_bool(item.invisible_flag())));
                    add_stat("Flags", to_utf8(format_binary(item.activation_flags())));
                    add_stat("OCB", to_utf8(format_bool(item.ocb())));
                }

                ImGui::EndTable();
            }
            if (ImGui::Button("Add to Route##addtoroute", ImVec2(-1, 30)))
            {
                on_add_to_route(_selected_item.value());
            }
            ImGui::Text("Triggered By");
            if (ImGui::BeginTable("##triggeredby", 3, ImGuiTableFlags_ScrollY, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                if (_selected_item.has_value())
                {
                    const auto& item = _selected_item.value();

                    for (auto& trigger : item.triggers())
                    {
                        auto trigger_ptr = trigger.lock();
                        if (!trigger_ptr)
                        {
                            continue;
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool selected = _selected_trigger.lock() == trigger_ptr;
                        if (ImGui::Selectable(std::to_string(trigger_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            _selected_trigger = trigger;
                            on_trigger_selected(trigger);
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(trigger_ptr->room()).c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(to_utf8(trigger_type_name(trigger_ptr->type())).c_str());
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void ItemsWindow::update(float delta)
    {
        _ui->update(delta);

        if (_tooltip_timer.has_value())
        {
            _tooltip_timer = _tooltip_timer.value() + delta;
            if (_tooltip_timer.value() > 0.6f)
            {
                _tooltip->set_visible(true);
                _tooltip_timer.reset();
            }
        }
    }

    void ItemsWindow::bind_tooltip()
    {
        using namespace ui;

        const auto items = _stats_list->items();
        for (uint32_t i = 0; i < items.size(); ++i)
        {
            const auto row = _stats_list->find<Listbox::Row>(Listbox::Names::row_name_format + std::to_string(i));
            const auto cell = row->find<Button>(Listbox::Row::Names::cell_name_format + "Name");
            const auto found = _tips.find(cell->text());
            if (found != _tips.end())
            {
                const auto tip = found->second;
                _token_store += cell->on_mouse_enter += [this, tip]()
                {
                    _tooltip_timer = 0.0f;
                    _tooltip->set_position(client_cursor_position(window()) + Point(0, 20));
                    _tooltip->set_text(tip);
                };

                _token_store += cell->on_mouse_move += [this, cell]()
                {
                    _tooltip->set_position(client_cursor_position(window()) + Point(0, 20));
                };

                _token_store += cell->on_mouse_leave += [this, cell]()
                {
                    _tooltip_timer.reset();
                    _tooltip->set_visible(false);
                };
            }
        }
    }
}
