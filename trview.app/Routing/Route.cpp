#include "Route.h"
#include <trview.app/Camera/ICamera.h>
#include <trview.common/Strings.h>
#include <trview.common/Maths.h>
#include <trview.common/Json.h>
#include <trview.app/Elements/ILevel.h>
#include <format>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        std::vector<uint8_t> from_base64(const std::string& text)
        {
            const auto b64 = to_utf16(text);
            DWORD required_length = 0;
            CryptStringToBinary(b64.c_str(), 0, CRYPT_STRING_BASE64, nullptr, &required_length, nullptr, nullptr);

            std::vector<uint8_t> data(required_length);
            if (required_length)
            {
                CryptStringToBinary(b64.c_str(), 0, CRYPT_STRING_BASE64, &data[0], &required_length, nullptr, nullptr);
            }
            return data;
        }

        std::string to_base64(const std::vector<uint8_t>& bytes)
        {
            if (bytes.empty())
            {
                return std::string();
            }

            DWORD required_length = 0;
            CryptBinaryToString(&bytes[0], static_cast<DWORD>(bytes.size()), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &required_length);

            std::vector<wchar_t> output_string(required_length);
            CryptBinaryToString(&bytes[0], static_cast<DWORD>(bytes.size()), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &output_string[0], &required_length);

            return to_utf8(std::wstring(&output_string[0]));
        }

        Vector3 load_vector3(const nlohmann::json& json, const std::string& name, Vector3 default_value)
        {
            if (!json.count(name))
            {
                return default_value;
            }

            auto vector_string = json[name].get<std::string>();
            std::stringstream stringstream(vector_string);
            std::vector<float> result;
            for (int i = 0; i < 3; ++i)
            {
                std::string substr;
                std::getline(stringstream, substr, ',');
                result.push_back(std::stof(substr));
            }
            return Vector3(result[0], result[1], result[2]);
        }

        nlohmann::ordered_json& find_element_case_insensitive(nlohmann::ordered_json& json, const std::string& target_key)
        {
            for (auto it = json.begin(); it != json.end(); ++it)
            {
                const auto& key = it.key();
                if (key.size() == target_key.size() &&
                    std::equal(key.begin(), key.end(), target_key.begin(),
                        [](const auto& l, const auto& r) { return std::toupper(l) == std::toupper(r); }))
                {
                    return *it;
                }
            }
            throw std::exception();
        }
    }

    Route::Route(std::unique_ptr<ISelectionRenderer> selection_renderer, const IWaypoint::Source& waypoint_source, const UserSettings& settings)
        : _selection_renderer(std::move(selection_renderer)), _waypoint_source(waypoint_source), _colour(settings.route_colour), _waypoint_colour(settings.waypoint_colour)
    {
    }

    Route& Route::operator=(const Route& other)
    {
        _waypoints = other._waypoints;
        _selected_index = other._selected_index;;
        _colour = other._colour;
        return *this;
    }

    void Route::add(const Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room)
    {
        add(position, normal, room, IWaypoint::Type::Position, 0u);
    }


    void Route::add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index)
    {
        _waypoints.push_back(_waypoint_source(position, normal, room, type, type_index, _colour, _waypoint_colour));
        set_unsaved(true);
    }

    Colour Route::colour() const
    {
        return _colour;
    }

    void Route::clear()
    {
        if (!_waypoints.empty())
        {
            set_unsaved(true);
        }
        _waypoints.clear();
        _selected_index = 0u;
    }

    void Route::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index)
    {
        if (index >= _waypoints.size())
        {
            return add(position, normal, room, IWaypoint::Type::Position, 0u);
        }
        insert(position, normal, room, index, IWaypoint::Type::Position, 0u);
        set_unsaved(true);
    }

    uint32_t Route::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room)
    {
        uint32_t index = next_index();
        insert(position, normal, room, index);
        return index;
    }

    void Route::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index, IWaypoint::Type type, uint32_t type_index)
    {
        _waypoints.insert(_waypoints.begin() + index, _waypoint_source(position, normal, room, type, type_index, _colour, _waypoint_colour));
        set_unsaved(true);
    }

    uint32_t Route::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index)
    {
        uint32_t index = next_index();
        insert(position, normal, room, index, type, type_index);
        return index;
    }

    bool Route::is_unsaved() const
    {
        return _is_unsaved;
    }

    void Route::move(int32_t from, int32_t to)
    {
        auto source = _waypoints[from];
        auto final_to = to;
        if (to > from)
        {
            final_to = to + 1;
        }
        _waypoints.insert(_waypoints.begin() + final_to, source);
        _waypoints.erase(_waypoints.begin() + from + ((from > final_to) ? 1 : 0));
        set_unsaved(true);
    }

    PickResult Route::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result;
        result.hit = false;

        for (uint32_t i = 0; i < _waypoints.size(); ++i)
        {
            const auto box = _waypoints[i]->bounding_box();
            
            float distance = 0;
            if (box.Intersects(position, direction, distance) && (!result.hit || distance < result.distance))
            {
                result.distance = distance;
                result.hit = true;
                result.index = i;
                result.position = position + direction * distance;
                result.type = PickResult::Type::Waypoint;
            }
        }

        return result;
    }

    void Route::remove(uint32_t index)
    {
        if (index >= _waypoints.size())
        {
            return;
        }
        _waypoints.erase(_waypoints.begin() + index);
        if (_selected_index >= index && _selected_index > 0)
        {
            --_selected_index;
        }
        set_unsaved(true);
    }

    void Route::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, bool show_selection)
    {
        for (std::size_t i = 0; i < _waypoints.size(); ++i)
        {
            auto& waypoint = _waypoints[i];
            waypoint->render(camera, texture_storage, _waypoint_colour);
            if (!_randomizer_enabled && i < _waypoints.size() - 1)
            {
                waypoint->render_join(*_waypoints[i + 1], camera, texture_storage, _colour);
            }
        }

        // Render selected waypoint...
        if (show_selection && _selected_index < _waypoints.size())
        {
            _selection_renderer->render(camera, texture_storage, *_waypoints[_selected_index], Colour::White);
        }
    }

    uint32_t Route::selected_waypoint() const
    {
        return _selected_index;
    }

    void Route::select_waypoint(uint32_t index)
    {
        _selected_index = index;
    }

    void Route::set_colour(const Colour& colour)
    {
        _colour = colour;
        for (auto& waypoint : _waypoints)
        {
            waypoint->set_route_colour(colour);
        }
        set_unsaved(true);
    }

    void Route::set_randomizer_enabled(bool enabled)
    {
        _randomizer_enabled = enabled;
    }

    void Route::set_waypoint_colour(const Colour& colour)
    {
        _waypoint_colour = colour;
        for (auto& waypoint : _waypoints)
        {
            waypoint->set_waypoint_colour(colour);
        }
        set_unsaved(true);
    }

    void Route::set_unsaved(bool value)
    {
        _is_unsaved = value;
    }

    Colour Route::waypoint_colour() const 
    {
        return _waypoint_colour;
    }

    const IWaypoint& Route::waypoint(uint32_t index) const
    {
        if (index < _waypoints.size())
        {
            return *_waypoints[index];
        }
        throw std::range_error("Waypoint index out of range");
    }

    IWaypoint& Route::waypoint(uint32_t index)
    {
        if (index < _waypoints.size())
        {
            return *_waypoints[index];
        }
        throw std::range_error("Waypoint index out of range");
    }

    uint32_t Route::waypoints() const
    {
        return static_cast<uint32_t>(_waypoints.size());
    }

    uint32_t Route::next_index() const
    {
        return _waypoints.empty() ? 0 : _selected_index + 1;
    }

    IWaypoint::WaypointRandomizerSettings import_randomizer_settings(const nlohmann::json& json, const RandomizerSettings& randomizer_settings)
    {
        IWaypoint::WaypointRandomizerSettings result;
        for (const auto& setting : randomizer_settings.settings)
        {
            if (!json.count(setting.first))
            {
                result[setting.first] = setting.second.default_value;
                continue;
            }

            switch (setting.second.type)
            {
                case RandomizerSettings::Setting::Type::Boolean:
                {
                    result[setting.first] = read_attribute<bool>(json, setting.first, std::get<bool>(setting.second.default_value));
                    break;
                }
                case RandomizerSettings::Setting::Type::Number:
                {
                    result[setting.first] = read_attribute<float>(json, setting.first, std::get<float>(setting.second.default_value));
                    break;
                }
                case RandomizerSettings::Setting::Type::String:
                {
                    // Covers the case where enum values are saved as integers.
                    if (!setting.second.options.empty() && json.count(setting.first) && json[setting.first].is_number())
                    {
                        const auto index = json[setting.first].get<int>();
                        result[setting.first] = setting.second.options[std::max(0, std::min<int>(index, setting.second.options.size() - 1))];
                    }
                    else
                    {
                        result[setting.first] = read_attribute<std::string>(json, setting.first, std::get<std::string>(setting.second.default_value));
                    }
                    break;
                }
            }
        }
        return result;
    }

    IWaypoint::WaypointRandomizerSettings import_trview_randomizer_settings(const nlohmann::json& json, const RandomizerSettings& randomizer_settings)
    {
        if (json.count("randomizer"))
        {
            return import_randomizer_settings(json["randomizer"], randomizer_settings);
        }
        return {};
    }

    std::shared_ptr<IRoute> import_rando_route(const IRoute::Source& route_source, const std::vector<uint8_t>& data, const ILevel* const level, const RandomizerSettings& randomizer_settings)
    {
        if (!level)
        {
            return nullptr;
        }

        auto json = nlohmann::ordered_json::parse(data.begin(), data.end());
        auto route = route_source();

        const auto level_filename = level->filename();
        auto trimmed = level_filename.substr(level_filename.find_last_of("/\\") + 1);
        for (const auto& location : find_element_case_insensitive(json, trimmed))
        {
            int x = location["X"];
            int y = location["Y"];
            int z = location["Z"];
            int room_number = location["Room"];

            // If the room space attribute is true then the coordinate must be transformed.
            if (read_attribute<bool>(location, "IsInRoomSpace", false))
            {
                if (room_number >= level->number_of_rooms())
                {
                    // Abandon adding this waypoint.
                    continue;
                }

                // Adjust coordinates by room position.
                auto room = level->room(room_number).lock();
                x += room->info().x;
                z += room->info().z;
                y = room->info().yBottom - y;
            }

            route->add(Vector3(x, y, z) / 1024.0f, Vector3::Down, room_number, IWaypoint::Type::Position, 0);
            auto& new_waypoint = route->waypoint(route->waypoints() - 1);
            new_waypoint.set_randomizer_settings(import_randomizer_settings(location, randomizer_settings));
        }

        route->set_unsaved(false);
        return route;
    }

    std::shared_ptr<IRoute> import_trview_route(const IRoute::Source& route_source, const std::vector<uint8_t>& data, const RandomizerSettings& randomizer_settings)
    {
        auto json = nlohmann::json::parse(data.begin(), data.end());

        auto route = route_source();
        if (json["colour"].is_string())
        {
            route->set_colour(from_colour_code(json["colour"].get<std::string>()));
        }

        if (json["waypoint_colour"].is_string())
        {
            route->set_waypoint_colour(from_colour_code(json["waypoint_colour"].get<std::string>()));
        }

        for (const auto& waypoint : json["waypoints"])
        {
            auto type_string = waypoint["type"].get<std::string>();
            IWaypoint::Type type = waypoint_type_from_string(type_string);
            Vector3 position = load_vector3(waypoint, "position", Vector3::Zero);
            Vector3 normal = load_vector3(waypoint, "normal", Vector3::Down);

            auto room = waypoint["room"].get<int>();
            auto index = waypoint["index"].get<int>();
            auto notes = waypoint["notes"].get<std::string>();

            route->add(position, normal, room, type, index);

            auto& new_waypoint = route->waypoint(route->waypoints() - 1);
            new_waypoint.set_notes(notes);
            new_waypoint.set_save_file(from_base64(waypoint.value("save", "")));
            new_waypoint.set_randomizer_settings(import_trview_randomizer_settings(waypoint, randomizer_settings));
        }

        route->set_unsaved(false);
        return route;
    }

    std::shared_ptr<IRoute> import_route(const IRoute::Source& route_source, const std::shared_ptr<IFiles>& files, const std::string& route_filename, const ILevel* const level, const RandomizerSettings& randomizer_settings, bool rando_import)
    {
        try
        {
            auto data = files->load_file(route_filename);
            if (!data.has_value())
            {
                return nullptr;
            }

            if (rando_import)
            {
                return import_rando_route(route_source, data.value(), level, randomizer_settings);
            }
            
            return import_trview_route(route_source, data.value(), randomizer_settings);
        }
        catch (std::exception& e)
        {
            MessageBoxA(0, e.what(), "Error", MB_OK);
            return nullptr;
        }
    }

    nlohmann::ordered_json try_load_route(std::shared_ptr<IFiles>& files, const std::string& route_filename)
    {
        try
        {
            auto data = files->load_file(route_filename);
            if (!data.has_value())
            {
                return nlohmann::ordered_json();
            }

            const auto data_bytes = data.value();
            return nlohmann::ordered_json::parse(data_bytes.begin(), data_bytes.end());
        }
        catch(...)
        {
        }
        
        return nlohmann::ordered_json();
    }

    /// <summary>
    /// Export randomizer settings direct to a randomizer locations file.
    /// </summary>
    /// <param name="json">The json to write to.</param>
    /// <param name="randomizer_settings">Randomzier setings and definitions.</param>
    /// <param name="waypoint">The waypoint to save.</param>
    void export_randomizer_settings(nlohmann::ordered_json& json, const RandomizerSettings& randomizer_settings, const IWaypoint& waypoint)
    {
        auto waypoint_settings = waypoint.randomizer_settings();
        for (const auto& setting : randomizer_settings.settings)
        {
            const auto value_to_set =
                waypoint_settings.find(setting.first) == waypoint_settings.end() ?
                setting.second.default_value : waypoint_settings[setting.first];

            if (!setting.second.always_output && value_to_set == setting.second.default_value)
            {
                continue;
            }

            switch (setting.second.type)
            {
                case RandomizerSettings::Setting::Type::Boolean:
                {
                    json[setting.first] = std::get<bool>(value_to_set);
                    break;
                }
                case RandomizerSettings::Setting::Type::String:
                {
                    json[setting.first] = std::get<std::string>(value_to_set);
                    break;
                }
                case RandomizerSettings::Setting::Type::Number:
                {
                    float value = std::get<float>(value_to_set);
                    if (fabs(round(value) - value) < FLT_EPSILON)
                    {
                        json[setting.first] = static_cast<int>(round(value));
                    }
                    else
                    {
                        json[setting.first] = value;
                    }
                    break;
                }
            }
        }
    }

    /// <summary>
    /// Export randomizer settings on waypoints so that they can be loaded by trview in the future.
    /// </summary>
    /// <param name="json">Json to write to.</param>
    /// <param name="settings">Randomizer settings and definitions.</param>
    /// <param name="waypoint">The waypoint to save.</param>
    void export_trview_randomizer_settings(nlohmann::json& json, const RandomizerSettings& randomizer_settings, const IWaypoint& waypoint)
    {
        nlohmann::ordered_json randomizer_node;
        export_randomizer_settings(randomizer_node, randomizer_settings, waypoint);
        if (!randomizer_node.is_null())
        {
            json["randomizer"] = randomizer_node;
        }
    }

    void export_randomizer_route(const IRoute& route, std::shared_ptr<IFiles>& files, const std::string& route_filename, const std::string& level_filename, const RandomizerSettings& randomizer_settings)
    {
        // Try to load the existing route
        nlohmann::ordered_json json = try_load_route(files, route_filename);

        std::vector<nlohmann::ordered_json> waypoints;
        for (uint32_t i = 0; i < route.waypoints(); ++i)
        {
            const IWaypoint& waypoint = route.waypoint(i);
            nlohmann::ordered_json waypoint_json;

            auto pos = waypoint.position();
            waypoint_json["X"] = static_cast<int>(pos.x * 1024);
            waypoint_json["Y"] = static_cast<int>(pos.y * 1024);
            waypoint_json["Z"] = static_cast<int>(pos.z * 1024);
            waypoint_json["Room"] = waypoint.room();
            export_randomizer_settings(waypoint_json, randomizer_settings, waypoint);

            waypoints.push_back(waypoint_json);
        }

        auto trimmed = level_filename.substr(level_filename.find_last_of("/\\") + 1);
        std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
        json[trimmed] = waypoints;
        files->save_file(route_filename, json.dump(2, ' '));
    }

    void export_trview_route(const IRoute& route, std::shared_ptr<IFiles>& files, const std::string& route_filename, const RandomizerSettings& randomizer_settings)
    {
        nlohmann::json json;
        json["colour"] = route.colour().code();
        json["waypoint_colour"] = route.waypoint_colour().code();

        std::vector<nlohmann::json> waypoints;

        for (uint32_t i = 0; i < route.waypoints(); ++i)
        {
            const IWaypoint& waypoint = route.waypoint(i);
            nlohmann::json waypoint_json;
            waypoint_json["type"] = waypoint_type_to_string(waypoint.type());

            const auto pos = waypoint.position();
            waypoint_json["position"] = std::format("{},{},{}", pos.x, pos.y, pos.z);
            const auto normal = waypoint.normal();
            waypoint_json["normal"] = std::format("{},{},{}", normal.x, normal.y, normal.z);
            waypoint_json["room"] = waypoint.room();
            waypoint_json["index"] = waypoint.index();
            waypoint_json["notes"] = waypoint.notes();

            if (waypoint.has_save())
            {
                waypoint_json["save"] = to_base64(waypoint.save_file());
            }
            export_trview_randomizer_settings(waypoint_json, randomizer_settings, waypoint);

            waypoints.push_back(waypoint_json);
        }

        json["waypoints"] = waypoints;
        files->save_file(route_filename, json.dump());
    }

    void export_route(const IRoute& route, std::shared_ptr<IFiles>& files, const std::string& route_filename, const std::string& level_filename, const RandomizerSettings& randomizer_settings, bool rando_export)
    {
        try
        {
            if (rando_export)
            {
                export_randomizer_route(route, files, route_filename, level_filename, randomizer_settings);
            }
            else
            {
                export_trview_route(route, files, route_filename, randomizer_settings);
            }
        }
        catch (...)
        {
        }
    }
}
