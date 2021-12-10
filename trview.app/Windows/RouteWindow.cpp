#include "RouteWindow.h"
#include <trview.app/Routing/Route.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Button.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>
#include <trview.ui/Layouts/StackLayout.h>
#include <trview.ui/Label.h>
#include <trview.ui/Layouts/GridLayout.h>
#include "../Resources/resource.h"

namespace trview
{
    using namespace ui;
    using namespace DirectX::SimpleMath;

    namespace
    {
        std::wstring pos_to_string(const Vector3& position)
        {
            return std::to_wstring(static_cast<int>(position.x * 1024)) + L", " +
                std::to_wstring(static_cast<int>(position.y * 1024)) + L", " + 
                std::to_wstring(static_cast<int>(position.z * 1024));
        }

        Listbox::Item make_item(const std::wstring& name, const std::wstring& value)
        {
            return Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };

        const uint32_t normal_min_height = 400;
    }

    const std::string RouteWindow::Names::colour = "colour";
    const std::string RouteWindow::Names::waypoints = "waypoints";
    const std::string RouteWindow::Names::delete_waypoint = "delete_waypoint";
    const std::string RouteWindow::Names::export_button = "export_button";
    const std::string RouteWindow::Names::import_button = "import_button";
    const std::string RouteWindow::Names::clear_save = "clear_save";
    const std::string RouteWindow::Names::notes_area = "notes_area";
    const std::string RouteWindow::Names::select_save_button = "select_save_button";
    const std::string RouteWindow::Names::waypoint_stats = "waypoint_stats";
    const std::string RouteWindow::Names::randomizer_group = "randomizer_group";
    const std::string RouteWindow::Names::randomizer_area = "randomizer_area";

    namespace Colours
    {
        const Colour Notes{ 1.0f, 0.20f, 0.20f, 0.20f };
        const Colour NotesTextArea{ 1.0f, 0.15f, 0.15f, 0.15f };
    }

    using namespace graphics;

    RouteWindow::RouteWindow(const IDeviceWindow::Source& device_window_source, const ui::render::IRenderer::Source& renderer_source,
        const ui::IInput::Source& input_source, const trview::Window& parent, const std::shared_ptr<IClipboard>& clipboard,
        const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files, const IBubble::Source& bubble_source,
        const std::shared_ptr<ui::ILoader>& ui_source, const std::shared_ptr<IShell>& shell)
        : CollapsiblePanel(device_window_source, renderer_source(Size(470, normal_min_height)), parent, L"trview.route", L"Route", input_source, Size(470, normal_min_height)), _clipboard(clipboard), _dialogs(dialogs), _files(files),
        _bubble(bubble_source(*_ui)), _shell(shell)
    {
        CollapsiblePanel::on_window_closed += IRouteWindow::on_window_closed;
        set_panels(create_left_panel(*ui_source), create_right_panel(*ui_source));
    }

    void RouteWindow::render(bool vsync)
    {
        CollapsiblePanel::render(vsync);
    }

    void RouteWindow::set_route(IRoute* route) 
    {
        _route = route;
        _selected_index = 0u;

        std::vector<Listbox::Item> items;
        for (uint32_t i = 0; i < _route->waypoints(); ++i)
        {
            items.push_back(create_listbox_item(i, _route->waypoint(i)));
        }
        _waypoints->set_items(items);
        load_waypoint_details(_selected_index);

        const auto colour = route->colour();
        _colour->set_text_colour(colour);
        _colour->set_text_background_colour(colour);
        _colour->set_selected_value(colour.code());
    }

    std::unique_ptr<Control> RouteWindow::create_left_panel(const ui::ILoader& ui_source)
    {
        auto left_panel = ui_source.load_from_resource(IDR_UI_ROUTE_WINDOW_LEFT_PANEL);
        _colour = left_panel->find<Dropdown>(Names::colour);
        _colour->set_dropdown_scope(_ui.get());

        _token_store += _colour->on_value_selected += [=](const auto& value)
        {
            const auto new_colour = from_named_colour(to_utf8(value));
            _colour->set_text_colour(new_colour);
            _colour->set_text_background_colour(new_colour);
            on_colour_changed(new_colour);
        };

        auto import = left_panel->find<Button>(Names::import_button);
        _token_store += import->on_click += [&]()
        {
            std::vector<IDialogs::FileFilter> filters{ { L"trview route", { L"*.tvr" } } };
            if (_randomizer_enabled)
            {
                filters.push_back({ L"Randomizer Locations", { L"*.json" } });
            }

            const auto filename = _dialogs->open_file(L"Import route", filters, OFN_FILEMUSTEXIST);
            if (filename.has_value())
            {
                on_route_import(filename.value().filename, filename.value().filter_index == 2);
            }
        };
        auto export_button = left_panel->find<Button>(Names::export_button);
        _token_store += export_button->on_click += [&]()
        {
            std::vector<IDialogs::FileFilter> filters { { L"trview route", { L"*.tvr" } } };
            uint32_t filter_index = 1;
            if (_randomizer_enabled)
            {
                filters.push_back({ L"Randomizer Locations", { L"*.json" } });
                filter_index = 2;
            }

            const auto filename = _dialogs->save_file(L"Export route", filters, filter_index);
            if (filename.has_value())
            {
                on_route_export(filename.value().filename, filename.value().filter_index == 2);
            }
        };

        // List box to show the waypoints in the route.
        _waypoints = left_panel->find<Listbox>(Names::waypoints);
        _token_store += _waypoints->on_item_selected += [&](const auto& item) {
            auto index = std::stoi(item.value(L"#"));
            load_waypoint_details(index);
            on_waypoint_selected(index);
        };
        _token_store += _waypoints->on_delete += [&]() {
            on_waypoint_deleted(_selected_index);
        };
        _token_store += on_size_changed += [&](const auto& size)
        {
            _waypoints->set_size(Size(200, size.height - 20));
        };

        return left_panel;
    }

    std::unique_ptr<Control> RouteWindow::create_right_panel(const ui::ILoader& ui_source)
    {
        const float panel_width = 270;

        auto right_panel = ui_source.load_from_resource(IDR_UI_ROUTE_WINDOW_RIGHT_PANEL);

        _stats = right_panel->find<Listbox>(Names::waypoint_stats);
        _token_store += _stats->on_item_selected += [&](const auto& item)
        {
            if (item.value(L"Name") == L"Room Position" || 
                item.value(L"Name") == L"Position")
            {
                _clipboard->write(window(), item.value(L"Value"));
                _bubble->show(client_cursor_position(window()) - Point(0, 20));
                return;
            }

            const auto index = _route->waypoint(_selected_index).index();
            switch (_selected_type)
            {
            case IWaypoint::Type::Entity:
                if (index < _all_items.size())
                {
                    on_item_selected(_all_items[index]);
                }
                break;
            case IWaypoint::Type::Trigger:
                if (index < _all_triggers.size())
                {
                    on_trigger_selected(_all_triggers[index]);
                }
                break;
            }
        };

        _select_save = right_panel->find<Button>(Names::select_save_button);
        _token_store += _select_save->on_click += [&]()
        {
            if (!(_route && _selected_index < _route->waypoints()))
            {
                return;
            }

            if (!_route->waypoint(_selected_index).has_save())
            {
                const auto filename = _dialogs->open_file(L"Select Save", { { L"Savegame File", { L"*.*" } } }, OFN_FILEMUSTEXIST);
                if (filename.has_value())
                {
                    // Load bytes from file.
                    try
                    {
                        const auto bytes = _files->load_file(filename.value().filename);
                        if (bytes.has_value() && !bytes.value().empty())
                        {
                            _route->waypoint(_selected_index).set_save_file(bytes.value());
                            _route->set_unsaved(true);
                            _select_save->set_text(L"SAVEGAME.0");
                        }
                    }
                    catch (...)
                    {
                        _dialogs->message_box(window(), L"Failed to attach save", L"Error", IDialogs::Buttons::OK);
                    }
                }
            }
            else
            {
                const auto filename = _dialogs->save_file(L"Export Save", { { L"Savegame File", { L"*.*" } } }, 1);
                if (filename.has_value())
                {
                    try
                    {
                        _files->save_file(filename.value().filename, _route->waypoint(_selected_index).save_file());
                    }
                    catch (...)
                    {
                        _dialogs->message_box(window(), L"Failed to export save", L"Error", IDialogs::Buttons::OK);
                    }
                }
            }
        };

        _clear_save = right_panel->find<Button>(Names::clear_save);
        _token_store += _clear_save->on_click += [&]()
        {
            if (!(_route && _selected_index < _route->waypoints()))
            {
                return;
            }

            auto& waypoint = _route->waypoint(_selected_index);
            if (waypoint.has_save())
            {
                waypoint.set_save_file({});
                _select_save->set_text(L"Attach Save");
                _route->set_unsaved(true);
            }
        };

        _delete_waypoint = right_panel->find<Button>(Names::delete_waypoint);
        _token_store += _delete_waypoint->on_click += [&]()
        {
            if (_route && _selected_index < _route->waypoints())
            {
                on_waypoint_deleted(_selected_index);
            }
        };

        _notes_area = right_panel->find<TextArea>(Names::notes_area);
        _token_store += _notes_area->on_text_changed += [&](const std::wstring& text)
        {
            if (_route && _selected_index < _route->waypoints())
            {
                if (_route->waypoint(_selected_index).notes() != text)
                {
                    _route->waypoint(_selected_index).set_notes(text);
                    _route->set_unsaved(true);
                }
            }
        };

        _rando_group = right_panel->find<ui::Window>(Names::randomizer_group);
        _rando_area = right_panel->find<ui::Window>(Names::randomizer_area);
        return right_panel;
    }

    Listbox::Item RouteWindow::create_listbox_item(uint32_t index, const IWaypoint& waypoint)
    {
        std::wstring type = waypoint_type_to_string(waypoint.type());
        if (waypoint.type() == IWaypoint::Type::Entity)
        {
            if (waypoint.index() < _all_items.size())
            {
                type = _all_items[waypoint.index()].type();
            }
            else
            {
                type = L"Invalid entity";
            }
        }
        else if (waypoint.type() == IWaypoint::Type::Trigger)
        {
            if (waypoint.index() < _all_triggers.size())
            {
                if (auto trigger = _all_triggers[waypoint.index()].lock())
                {
                    type = trigger_type_name(trigger->type());
                }
            }
            else
            {
                type = L"Invalid trigger";
            }
        }
        return { {{ L"#", std::to_wstring(index) },
                 { L"Type", type}} };
    }

    void RouteWindow::load_waypoint_details(uint32_t index)
    {
        if (index >= _route->waypoints())
        {
            _stats->set_items({});
            _notes_area->set_text(L"");
            _select_save->set_visible(false);
            _clear_save->set_visible(false);
            _delete_waypoint->set_visible(false);
            return;
        }

        _select_save->set_visible(true);
        _clear_save->set_visible(true);
        _delete_waypoint->set_visible(true);

        const auto& waypoint = _route->waypoint(index);

        auto get_room_pos = [&waypoint, this]()
        {
            if (waypoint.room() < _all_rooms.size())
            {
                const auto room = _all_rooms[waypoint.room()].lock();
                if (!room)
                {
                    return waypoint.position();
                }
                const auto info = room->info();
                const Vector3 bottom_left = Vector3(info.x, info.yBottom, info.z) / trlevel::Scale_X;
                return waypoint.position() - bottom_left;
            }
            return waypoint.position();
        };

        
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", waypoint_type_to_string(waypoint.type())));
        stats.push_back(make_item(L"Position", pos_to_string(waypoint.position())));
        stats.push_back(make_item(L"Room", std::to_wstring(waypoint.room())));
        stats.push_back(make_item(L"Room Position", pos_to_string(get_room_pos())));

        _selected_type = waypoint.type();
        _selected_index = index;

        if (waypoint.type() == IWaypoint::Type::Entity || waypoint.type() == IWaypoint::Type::Trigger)
        {
            stats.push_back(make_item(L"Target Index", std::to_wstring(waypoint.index())));
            if (waypoint.type() == IWaypoint::Type::Entity)
            {
                std::wstring type = L"Invalid entity";
                if (waypoint.index() < _all_items.size())
                {
                    type = _all_items[waypoint.index()].type();
                }
                stats.push_back(make_item(L"Entity", type));
            }
            else if (waypoint.type() == IWaypoint::Type::Trigger)
            {
                std::wstring type = L"Invalid trigger";
                if (waypoint.index() < _all_triggers.size())
                {
                    if (auto trigger = _all_triggers[waypoint.index()].lock())
                    {
                        type = trigger_type_name(trigger->type());
                    }
                }
                stats.push_back(make_item(L"Trigger Type", type));
            }
        }

        _stats->set_items(stats);
        _rando_group->set_visible(_randomizer_enabled);
        update_minimum_height();
        load_randomiser_settings(waypoint);

        _notes_area->set_text(waypoint.notes());

        if (waypoint.has_save())
        {
            _select_save->set_text(L"SAVEGAME.0");
        }
        else
        {
            _select_save->set_text(L"Attach Save");
        }
    }

    void RouteWindow::select_waypoint(uint32_t index)
    {
        _selected_index = index;

        if (!_route)
        {
            return;
        }

        if (index < _route->waypoints())
        {
            _waypoints->set_selected_item(create_listbox_item(index, _route->waypoint(index)));
            load_waypoint_details(index);
        }
    }

    /// Set the items to that are in the level.
    /// @param items The items to show.
    void RouteWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }

    void RouteWindow::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _all_rooms = rooms;
    }

    void RouteWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
    }

    void RouteWindow::focus()
    {
        SetForegroundWindow(window());
    }

    void RouteWindow::update(float delta)
    {
        _ui->update(delta);
    }

    void RouteWindow::set_randomizer_enabled(bool value)
    {
        _randomizer_enabled = value;
        _rando_group->set_visible(_randomizer_enabled);
        update_minimum_height();
    }

    void RouteWindow::set_randomizer_settings(const RandomizerSettings& settings)
    {
        _randomizer_settings = settings;

        const float panel_width = 270;

        const auto num_bools = settings.settings_of_type(RandomizerSettings::Setting::Type::Boolean);
        const auto rows = static_cast<int>(std::ceil(num_bools / 2.0));
        auto grid = _rando_area->add_child(std::make_unique<ui::Window>(Size(panel_width, rows * 25), Colours::Notes));
        grid->set_layout(std::make_unique<GridLayout>(2, rows));

        for (const auto& x : _randomizer_settings.settings)
        {
            const std::string name = x.first;
            switch (x.second.type)
            {
                case RandomizerSettings::Setting::Type::Boolean:
                {
                    auto checkbox = grid->add_child(std::make_unique<Checkbox>(Colour::Transparent, to_utf16(x.second.display)));
                    checkbox->set_name(name);
                    _token_store += checkbox->on_state_changed += [this, name](bool state)
                    {
                        if (_route && _selected_index < _route->waypoints())
                        {
                            auto& waypoint = _route->waypoint(_selected_index);
                            auto settings = waypoint.randomizer_settings();
                            settings[name] = state;
                            waypoint.set_randomizer_settings(settings);
                            _route->set_unsaved(true);
                        }
                    };
                    break;
                }
                case RandomizerSettings::Setting::Type::String:
                {
                    // TODO: Label for text element (Dropdown or otherwise)
                    auto string_area = _rando_area->add_child(std::make_unique<ui::Window>(Size(panel_width, 20), Colours::Notes));
                    string_area->set_layout(std::make_unique<StackLayout>(0.0f, StackLayout::Direction::Horizontal));
                    string_area->add_child(std::make_unique<Label>(Size(100, 20), Colours::Notes, to_utf16(x.second.display), 8, TextAlignment::Left, ParagraphAlignment::Centre));

                    const auto update_setting = [this, name](const std::wstring& value)
                    {
                        if (_route && _selected_index < _route->waypoints())
                        {
                            auto& waypoint = _route->waypoint(_selected_index);
                            auto settings = waypoint.randomizer_settings();
                            settings[name] = to_utf8(value);
                            waypoint.set_randomizer_settings(settings);
                            _route->set_unsaved(true);
                        }
                    };

                    if (x.second.options.empty())
                    {
                        auto text_area = string_area->add_child(std::make_unique<TextArea>(Size(panel_width / 2.0f, 20), Colours::NotesTextArea, Colour::White, _shell));
                        text_area->set_name(x.first);
                        text_area->set_mode(TextArea::Mode::SingleLine);
                        _token_store += text_area->on_text_changed += update_setting;
                    }
                    else
                    {
                        auto dropdown = string_area->add_child(std::make_unique<Dropdown>(Size(panel_width / 2.0f, 20)));
                        dropdown->set_name(x.first);

                        std::vector<std::wstring> options;
                        for (const auto& o : x.second.options)
                        {
                            options.push_back(to_utf16(std::get<std::string>(o)));
                        }

                        dropdown->set_values(options);
                        dropdown->set_dropdown_scope(_ui.get());
                        _token_store += dropdown->on_value_selected += update_setting;
                    }
                    break;
                }
                case RandomizerSettings::Setting::Type::Number:
                {
                    auto string_area = _rando_area->add_child(std::make_unique<ui::Window>(Size(panel_width, 20), Colours::Notes));
                    string_area->set_layout(std::make_unique<StackLayout>(0.0f, StackLayout::Direction::Horizontal));
                    string_area->add_child(std::make_unique<Label>(Size(100, 20), Colours::Notes, to_utf16(x.second.display), 8, TextAlignment::Left, ParagraphAlignment::Centre));
                    auto number_area = string_area->add_child(std::make_unique<TextArea>(Size(panel_width / 2.0f, 20), Colours::NotesTextArea, Colour::White, _shell));
                    number_area->set_mode(TextArea::Mode::SingleLine);
                    number_area->set_name(x.first);

                    const auto update_number = [this, name, number_area](const std::wstring& text)
                    {
                        if (_route && _selected_index < _route->waypoints())
                        {
                            auto& waypoint = _route->waypoint(_selected_index);
                            auto settings = waypoint.randomizer_settings();
                            try
                            {
                                const auto value = std::stof(text);
                                settings[name] = value;
                                waypoint.set_randomizer_settings(settings);
                                _route->set_unsaved(true);
                            }
                            catch (...)
                            {
                                const auto value_to_set =
                                    settings.find(name) == settings.end() ?
                                    _randomizer_settings.settings[name].default_value : settings[name];
                                number_area->set_text(std::to_wstring(std::get<float>(value_to_set)));
                            }
                        }
                    };

                    _token_store += number_area->on_enter += update_number;
                    _token_store += number_area->on_focus_lost += [this, name, number_area, update_number]()
                    {
                        update_number(number_area->text());
                    };
                    break;
                }
            }
        }

        update_minimum_height();
    }

    void RouteWindow::load_randomiser_settings(const IWaypoint& waypoint)
    {
        auto waypoint_settings = waypoint.randomizer_settings();
        for (const auto& s : _randomizer_settings.settings)
        {
            const auto value_to_set =
                waypoint_settings.find(s.first) == waypoint_settings.end() ?
                s.second.default_value : waypoint_settings[s.first];

            switch (s.second.type)
            {
                case RandomizerSettings::Setting::Type::Boolean:
                {
                    auto checkbox = _rando_area->find<Checkbox>(s.first);
                    checkbox->set_state(std::get<bool>(value_to_set));
                    break;
                }
                case RandomizerSettings::Setting::Type::String:
                {
                    if (s.second.options.empty())
                    {
                        auto text = _rando_area->find<TextArea>(s.first);
                        text->set_text(to_utf16(std::get<std::string>(value_to_set)));
                    }
                    else
                    {
                        auto dropdown = _rando_area->find<Dropdown>(s.first);
                        dropdown->set_selected_value(to_utf16(std::get<std::string>(value_to_set)));
                    }
                    break;
                }
                case RandomizerSettings::Setting::Type::Number:
                {
                    auto text = _rando_area->find<TextArea>(s.first);
                    text->set_text(std::to_wstring(std::get<float>(value_to_set)));
                    break;
                }
            }
        }
    }

    void RouteWindow::update_minimum_height()
    {
        if (_randomizer_enabled)
        {
            set_minimum_height(_rando_group->absolute_position().y + _rando_group->size().height);
        }
        else
        {
            set_minimum_height(normal_min_height);
        }
    }
}
