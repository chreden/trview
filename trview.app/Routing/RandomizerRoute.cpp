#include "RandomizerRoute.h"
#include "../Elements/ILevel.h"
#include "../Settings/UserSettings.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        std::string trimmed_level_name(const std::string& input)
        {
            return input.substr(input.find_last_of("/\\") + 1);
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

        std::shared_ptr<IRandomizerRoute> import_rando_route(const IRandomizerRoute::Source& route_source, const std::vector<uint8_t>& data, const RandomizerSettings& randomizer_settings)
        {
            auto json = nlohmann::ordered_json::parse(data.begin(), data.end());
            auto route = route_source();

            for (const auto& level : json.items())
            {
                for (const auto& location : level.value())
                {
                    int x = location["X"];
                    int y = location["Y"];
                    int z = location["Z"];
                    uint32_t room_number = location["Room"];

                    route->add(level.key(), Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) / 1024.0f, Vector3::Down, room_number);
                    if (auto new_waypoint = route->waypoint(route->waypoints() - 1).lock())
                    {
                        new_waypoint->set_randomizer_settings(import_randomizer_settings(location, randomizer_settings));
                    }
                }
            }

            route->set_unsaved(false);
            return route;
        }

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
    }

    IRandomizerRoute::~IRandomizerRoute()
    {
    }

    RandomizerRoute::RandomizerRoute(const std::shared_ptr<IRoute>& inner_route, const IWaypoint::Source& waypoint_source)
        : _route(inner_route), _waypoint_source(waypoint_source)
    {
    }

    std::shared_ptr<IWaypoint> RandomizerRoute::add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room)
    {
        auto result = _route->add(position, normal, room);
        update_waypoints();
        return result;
    }

    std::shared_ptr<IWaypoint> RandomizerRoute::add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index)
    {
        auto result = _route->add(position, normal, room, type, type_index);
        update_waypoints();
        return result;
    }

    std::shared_ptr<IWaypoint> RandomizerRoute::add(const std::shared_ptr<IWaypoint>& waypoint)
    {
        auto result = _route->add(waypoint);
        update_waypoints();
        return result;
    }

    std::shared_ptr<IWaypoint> RandomizerRoute::add(const std::string& level_name, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room_number)
    {
        auto waypoint = _waypoint_source(position, normal, room_number, IWaypoint::Type::Position, 0, _route->colour(), _route->waypoint_colour());
        _waypoints[level_name].push_back(waypoint);
        return waypoint;
    }

    void RandomizerRoute::clear()
    {
        _route->clear();
        update_waypoints();
    }

    Colour RandomizerRoute::colour() const
    {
        return _route->colour();
    }

    std::optional<std::string> RandomizerRoute::filename() const
    {
        return _filename;
    }

    std::vector<std::string> RandomizerRoute::filenames() const
    {
        return _waypoints | std::views::keys | std::ranges::to<std::vector>();
    }

    void RandomizerRoute::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index)
    {
        _route->insert(position, normal, room, index);
        update_waypoints();
    }

    uint32_t RandomizerRoute::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room)
    {
        auto result = _route->insert(position, normal, room);
        update_waypoints();
        return result;
    }

    void RandomizerRoute::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index, IWaypoint::Type type, uint32_t type_index)
    {
        _route->insert(position, normal, room, index, type, type_index);
        update_waypoints();
    }

    uint32_t RandomizerRoute::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index)
    {
        auto result = _route->insert(position, normal, room, type, type_index);
        update_waypoints();
        return result;
    }

    bool RandomizerRoute::is_unsaved() const
    {
        return _route->is_unsaved();
    }

    std::weak_ptr<ILevel> RandomizerRoute::level() const
    {
        return _route->level();
    }

    void RandomizerRoute::move(int32_t from, int32_t to)
    {
        _route->move(from, to);
        update_waypoints();
    }

    PickResult RandomizerRoute::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        return _route->pick(position, direction);
    }

    void RandomizerRoute::remove(uint32_t index)
    {
        _route->remove(index);
        update_waypoints();
    }

    void RandomizerRoute::remove(const std::shared_ptr<IWaypoint>& waypoint)
    {
        _route->remove(waypoint);
        update_waypoints();
    }

    void RandomizerRoute::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, bool show_selection)
    {
        return _route->render(camera, texture_storage, show_selection);
    }

    void RandomizerRoute::save(const std::shared_ptr<IFiles>& files, const UserSettings& settings)
    {
        if (!_filename)
        {
            return;
        }

        // TODO: Standard route saving.
        //. nlohmann::ordered_json json = try_load_route(files, route_filename);
        nlohmann::ordered_json json;

        // Sync the waypoints for the current route to the storage.
        update_waypoints();

        for (const auto& [level, waypoints] : _waypoints)
        {
            std::vector<nlohmann::ordered_json> waypoints_element;
            for (const auto& waypoint : waypoints)
            {
                nlohmann::ordered_json waypoint_json;

                auto pos = waypoint->position();
                waypoint_json["X"] = static_cast<int>(pos.x * 1024);
                waypoint_json["Y"] = static_cast<int>(pos.y * 1024);
                waypoint_json["Z"] = static_cast<int>(pos.z * 1024);
                waypoint_json["Room"] = waypoint->room();
                export_randomizer_settings(waypoint_json, settings.randomizer, *waypoint);

                waypoints_element.push_back(waypoint_json);
            }

            json[level] = waypoints_element;
            files->save_file(_filename.value(), json.dump(2, ' '));
        }

        _route->set_unsaved(false);
    }

    uint32_t RandomizerRoute::selected_waypoint() const
    {
        return _route->selected_waypoint();
    }

    void RandomizerRoute::select_waypoint(uint32_t index)
    {
        return _route->select_waypoint(index);
    }

    void RandomizerRoute::set_colour(const Colour& colour)
    {
        return _route->set_colour(colour);
    }

    void RandomizerRoute::set_filename(const std::string& filename)
    {
        _filename = filename;
    }

    void RandomizerRoute::set_level(const std::weak_ptr<ILevel>& level)
    {
        _route->clear();
        if (auto new_level = level.lock())
        {
            const auto found = _waypoints.find(trimmed_level_name(new_level->filename()));
            if (found != _waypoints.end())
            {
                std::ranges::for_each(found->second, [this](auto&& w) { _route->add(w); });
            }
        }
        _route->set_level(level);
        _route->set_unsaved(false);
    }

    void RandomizerRoute::set_randomizer_enabled(bool enabled)
    {
        return _route->set_randomizer_enabled(enabled);
    }

    void RandomizerRoute::set_waypoint_colour(const Colour& colour)
    {
        return _route->set_waypoint_colour(colour);
    }

    void RandomizerRoute::set_unsaved(bool value)
    {
        return _route->set_unsaved(value);
    }

    Colour RandomizerRoute::waypoint_colour() const
    {
        return _route->waypoint_colour();
    }

    std::weak_ptr<IWaypoint> RandomizerRoute::waypoint(uint32_t index) const
    {
        return _route->waypoint(index);
    }

    uint32_t RandomizerRoute::waypoints() const
    {
        return _route->waypoints();
    }

    void RandomizerRoute::update_waypoints()
    {
        if (auto current_level = _route->level().lock())
        {
            std::vector<std::shared_ptr<IWaypoint>> waypoints;
            for (auto i = 0u; i < _route->waypoints(); ++i)
            {
                if (auto waypoint = _route->waypoint(i).lock())
                {
                    waypoints.push_back(waypoint);
                }
                _waypoints[trimmed_level_name(current_level->filename())] = waypoints;
            }
        }
    }

    std::shared_ptr<IRoute> import_randomizer_route(const IRandomizerRoute::Source& route_source, const std::shared_ptr<IFiles>& files, const std::string& route_filename, const RandomizerSettings& randomizer_settings)
    {
        try
        {
            auto data = files->load_file(route_filename);
            if (!data.has_value())
            {
                return nullptr;
            }

            return import_rando_route(route_source, data.value(), randomizer_settings);
        }
        catch (std::exception& e)
        {
            MessageBoxA(0, e.what(), "Error", MB_OK);
            return nullptr;
        }
    }
}
