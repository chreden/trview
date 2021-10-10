#include "Route.h"
#include <trview.app/Camera/ICamera.h>
#include <trview.common/Strings.h>
#include <trview.common/Maths.h>

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
    }

    Route::Route(std::unique_ptr<ISelectionRenderer> selection_renderer, const IWaypoint::Source& waypoint_source)
        : _selection_renderer(std::move(selection_renderer)), _waypoint_source(waypoint_source)
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
        _waypoints.push_back(_waypoint_source(position, normal, room, type, type_index, _colour));
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
        _waypoints.insert(_waypoints.begin() + index, _waypoint_source(position, normal, room, type, type_index, _colour));
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

    void Route::render(const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        for (std::size_t i = 0; i < _waypoints.size(); ++i)
        {
            auto& waypoint = _waypoints[i];
            waypoint->render(camera, texture_storage, Color(1.0f, 1.0f, 1.0f));

            if (i < _waypoints.size() - 1)
            {
                waypoint->render_join(*_waypoints[i + 1], camera, texture_storage, _colour);
            }
        }

        // Render selected waypoint...
        if (_selected_index < _waypoints.size())
        {
            _selection_renderer->render(camera, texture_storage, *_waypoints[_selected_index], Color(1.0f, 1.0f, 1.0f));
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

    void Route::set_unsaved(bool value)
    {
        _is_unsaved = value;
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

    std::shared_ptr<IRoute> import_route(const IRoute::Source& route_source, const std::shared_ptr<IFiles>& files, const std::string& filename)
    {
        try
        {
            auto data = files->load_file(filename);
            if (!data.has_value())
            {
                return nullptr;
            }

            auto json = nlohmann::json::parse(data.value().begin(), data.value().end());

            auto route = route_source();
            if (json["colour"].is_string())
            {
                route->set_colour(named_colour(to_utf16(json["colour"].get<std::string>())));
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
                new_waypoint.set_notes(to_utf16(notes));
                new_waypoint.set_save_file(from_base64(waypoint.value("save", "")));

                if (type == IWaypoint::Type::RandoLocation)
                {
                    new_waypoint.set_requires_glitch(waypoint["RequiresGlitch"].get<bool>());
                    new_waypoint.set_difficulty(waypoint["Difficulty"].get<std::string>());
                    new_waypoint.set_is_in_room_space(waypoint["IsInRoomSpace"].get<bool>());
                    new_waypoint.set_is_item(waypoint["IsItem"].get<bool>());
                    new_waypoint.set_vehicle_required(waypoint["VehicleRequired"].get<bool>());
                }
            }

            route->set_unsaved(false);
            return route;
        }
        catch (std::exception&)
        {
            return nullptr;
        }
    }

    bool is_randomizer_route(const IRoute& route) 
    {
        for (uint32_t i = 0; i < route.waypoints(); ++i)
        {
            if (route.waypoint(i).type() == IWaypoint::Type::RandoLocation)
            {
                return true;
            }
        }
        return false;
    }

    void export_route(const IRoute& route, std::shared_ptr<IFiles>& files, const std::string& route_filename, const std::string& level_filename, bool rando_export)
    {
        try
        {
            if (rando_export)
            {
                nlohmann::ordered_json json;
                std::vector<nlohmann::ordered_json> waypoints;
                for (uint32_t i = 0; i < route.waypoints(); ++i)
                {
                    const IWaypoint& waypoint = route.waypoint(i);
                    nlohmann::ordered_json waypoint_json;

                    if (waypoint.type() == IWaypoint::Type::RandoLocation)
                    {
                        auto pos = waypoint.position();
                        waypoint_json["X"] = std::to_string((int)(pos.x * 1024));
                        waypoint_json["Y"] = std::to_string((int)(pos.y * 1024));
                        waypoint_json["Z"] = std::to_string((int)(pos.z * 1024));
                        waypoint_json["Room"] = waypoint.room();
                        if (waypoint.requires_glitch())
                        {
                            waypoint_json["RequiresGlitch"] = waypoint.requires_glitch();
                        }
                        if (waypoint.difficulty() != "Easy")
                        {
                            waypoint_json["Difficulty"] = waypoint.difficulty();
                        }
                        if (!waypoint.is_in_room_space())
                        {
                            waypoint_json["IsInRoomSpace"] = waypoint.is_in_room_space();
                        }
                        if (waypoint.is_item())
                        {
                            waypoint_json["IsItem"] = waypoint.is_item();
                        }
                        if (waypoint.vehicle_required())
                        {
                            waypoint_json["VehicleRequired"] = waypoint.vehicle_required();
                        }

                        waypoints.push_back(waypoint_json);
                    }
                }

                auto trimmed = level_filename.substr(level_filename.find_last_of("/\\") + 1);
                json[trimmed] = waypoints;
                files->save_file(route_filename, json.dump(1, '\t'));
            }
            else
            {
                nlohmann::json json;
                json["colour"] = to_utf8(route.colour().name());

                std::vector<nlohmann::json> waypoints;

                for (uint32_t i = 0; i < route.waypoints(); ++i)
                {
                    const IWaypoint& waypoint = route.waypoint(i);
                    nlohmann::json waypoint_json;
                    waypoint_json["type"] = to_utf8(waypoint_type_to_string(waypoint.type()));

                    std::stringstream pos_string;
                    auto pos = waypoint.position();
                    pos_string << pos.x << "," << pos.y << "," << pos.z;
                    waypoint_json["position"] = pos_string.str();
                    std::stringstream normal_string;
                    auto normal = waypoint.normal();
                    normal_string << normal.x << "," << normal.y << "," << normal.z;
                    waypoint_json["normal"] = normal_string.str();
                    waypoint_json["room"] = waypoint.room();
                    waypoint_json["index"] = waypoint.index();
                    waypoint_json["notes"] = to_utf8(waypoint.notes());

                    if (waypoint.has_save())
                    {
                        waypoint_json["save"] = to_base64(waypoint.save_file());
                    }

                    if (waypoint.type() == IWaypoint::Type::RandoLocation)
                    {
                        waypoint_json["RequiresGlitch"] = waypoint.requires_glitch();
                        waypoint_json["Difficulty"] = waypoint.difficulty();
                        waypoint_json["IsInRoomSpace"] = waypoint.is_in_room_space();
                        waypoint_json["IsItem"] = waypoint.is_item();
                        waypoint_json["VehicleRequired"] = waypoint.vehicle_required();
                    }

                    waypoints.push_back(waypoint_json);
                }

                json["waypoints"] = waypoints;
                files->save_file(route_filename, json.dump());
            }
        }
        catch (...)
        {
        }
    }
}
