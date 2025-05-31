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

        std::shared_ptr<IRoute> import_trview_route(const IRoute::Source& route_source, const std::vector<uint8_t>& data)
        {
            auto route = route_source(IRoute::FileData {.data = data });
            route->set_unsaved(false);
            return route;
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
            catch (...)
            {
            }

            return nlohmann::ordered_json();
        }
    }

    IRoute::~IRoute()
    {
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

    std::shared_ptr<IWaypoint> Route::add(const Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room)
    {
        return add(position, normal, room, IWaypoint::Type::Position, 0u);
    }

    std::shared_ptr<IWaypoint> Route::add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index)
    {
        return add(_waypoint_source(position, normal, room, type, type_index, _colour, _waypoint_colour));
    }

    std::shared_ptr<IWaypoint> Route::add(const std::shared_ptr<IWaypoint>& waypoint)
    {
        _waypoints.push_back(waypoint);
        bind_waypoint(*waypoint);
        set_unsaved(true);
        return waypoint;
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
        std::ranges::for_each(_waypoints, [this](auto&& w) { unbind_waypoint(*w); });
        _waypoints.clear();
        _selected_index = 0u;
    }

    std::optional<std::string> Route::filename() const
    {
        return _filename;
    }

    void Route::insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index)
    {
        if (index >= _waypoints.size())
        {
            add(position, normal, room, IWaypoint::Type::Position, 0u);
            return;
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
        auto waypoint = _waypoint_source(position, normal, room, type, type_index, _colour, _waypoint_colour);
        bind_waypoint(*waypoint);
        _waypoints.insert(_waypoints.begin() + index, waypoint);
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

    std::weak_ptr<ILevel> Route::level() const
    {
        return _level;
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
                result.waypoint = _waypoints[i];
                result.position = position + direction * distance;
                result.type = PickResult::Type::Waypoint;
                result.waypoint_index = i;
            }
        }

        return result;
    }

    void Route::reload(const std::shared_ptr<IFiles>& files, const UserSettings&)
    {
        if (!_filename)
        {
            return;
        }

        if (auto data = files->load_file(_filename.value()))
        {
            import(data.value());
        }
        set_unsaved(false);
    }

    void Route::remove(uint32_t index)
    {
        if (index >= _waypoints.size())
        {
            return;
        }
        unbind_waypoint(*_waypoints[index]);
        _waypoints.erase(_waypoints.begin() + index);
        if (_selected_index >= index && _selected_index > 0)
        {
            --_selected_index;
        }
        set_unsaved(true);
    }

    void Route::remove(const std::shared_ptr<IWaypoint>& waypoint)
    {
        auto found = std::ranges::find(_waypoints, waypoint);
        if (found == _waypoints.end())
        {
            return;
        }
        remove(static_cast<uint32_t>(found - _waypoints.begin()));
    }

    void Route::render(const ICamera& camera, bool show_selection)
    {
        for (std::size_t i = 0; i < _waypoints.size(); ++i)
        {
            auto& waypoint = _waypoints[i];
            waypoint->render(camera, _waypoint_colour);
            if (_show_route_line && i < _waypoints.size() - 1)
            {
                waypoint->render_join(*_waypoints[i + 1], camera, _colour);
            }
        }

        // Render selected waypoint...
        if (show_selection && _selected_index < _waypoints.size())
        {
            _selection_renderer->render(camera, *_waypoints[_selected_index], Colour::White);
        }
    }

    void Route::save(const std::shared_ptr<IFiles>& files, const UserSettings&)
    {
        if (!_filename)
        {
            return;
        }

        nlohmann::json json;
        json["colour"] = colour().code();
        json["waypoint_colour"] = waypoint_colour().code();

        std::vector<nlohmann::json> waypoints;

        for (const auto& waypoint : _waypoints) 
        {
            nlohmann::json waypoint_json;
            waypoint_json["type"] = waypoint_type_to_string(waypoint->type());

            const auto pos = waypoint->position();
            waypoint_json["position"] = std::format("{},{},{}", pos.x, pos.y, pos.z);
            const auto normal = waypoint->normal();
            waypoint_json["normal"] = std::format("{},{},{}", normal.x, normal.y, normal.z);
            waypoint_json["room"] = waypoint->room();
            waypoint_json["index"] = waypoint->index();
            waypoint_json["notes"] = waypoint->notes();

            if (waypoint->has_save())
            {
                waypoint_json["save"] = to_base64(waypoint->save_file());
            }

            waypoints.push_back(waypoint_json);
        }

        json["waypoints"] = waypoints;
        files->save_file(_filename.value(), json.dump());
    }

    void Route::save_as(const std::shared_ptr<IFiles>& files, const std::string& filename, const UserSettings&)
    {
        nlohmann::json json;
        json["colour"] = colour().code();
        json["waypoint_colour"] = waypoint_colour().code();

        std::vector<nlohmann::json> waypoints;

        for (const auto& waypoint : _waypoints)
        {
            nlohmann::json waypoint_json;
            waypoint_json["type"] = waypoint_type_to_string(waypoint->type());

            const auto pos = waypoint->position();
            waypoint_json["position"] = std::format("{},{},{}", pos.x, pos.y, pos.z);
            const auto normal = waypoint->normal();
            waypoint_json["normal"] = std::format("{},{},{}", normal.x, normal.y, normal.z);
            waypoint_json["room"] = waypoint->room();
            waypoint_json["index"] = waypoint->index();
            waypoint_json["notes"] = waypoint->notes();

            if (waypoint->has_save())
            {
                waypoint_json["save"] = to_base64(waypoint->save_file());
            }

            waypoints.push_back(waypoint_json);
        }

        json["waypoints"] = waypoints;
        files->save_file(filename, json.dump());
    }

    uint32_t Route::selected_waypoint() const
    {
        return _selected_index;
    }

    void Route::select_waypoint(const std::weak_ptr<IWaypoint>& waypoint)
    {
        if (const auto waypoint_ptr = waypoint.lock())
        {
            const auto iter = std::ranges::find(_waypoints, waypoint_ptr);
            if (iter != _waypoints.end())
            {
                _selected_index = static_cast<uint32_t>(std::distance(_waypoints.begin(), iter));
            }
        }
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

    void Route::set_filename(const std::string& filename)
    {
        _filename = filename;
    }

    void Route::set_level(const std::weak_ptr<ILevel>& level)
    {
        _level = level;
        bind_waypoint_targets();
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
        on_changed();
    }

    void Route::set_show_route_line(bool show)
    {
        _show_route_line = show;
        on_changed();
    }

    bool Route::show_route_line() const
    {
        return _show_route_line;
    }

    Colour Route::waypoint_colour() const 
    {
        return _waypoint_colour;
    }

    std::weak_ptr<IWaypoint> Route::waypoint(uint32_t index) const
    {
        if (index < _waypoints.size())
        {
            return _waypoints[index];
        }
        return {};
    }

    uint32_t Route::waypoints() const
    {
        return static_cast<uint32_t>(_waypoints.size());
    }

    uint32_t Route::next_index() const
    {
        return _waypoints.empty() ? 0 : _selected_index + 1;
    }

    void Route::bind_waypoint_targets()
    {
        const auto level = _level.lock();
        for (auto& waypoint : _waypoints)
        {
            switch (waypoint->type())
            {
                case IWaypoint::Type::Entity:
                {
                    if (level)
                    {
                        waypoint->set_item(level->item(waypoint->index()));
                    }
                    else
                    {
                        waypoint->set_item({});
                    }
                    break;
                }
                case IWaypoint::Type::Trigger:
                {
                    if (level)
                    {
                        waypoint->set_trigger(level->trigger(waypoint->index()));
                    }
                    else
                    {
                        waypoint->set_trigger({});
                    }
                    break;
                }
            }
        }
    }

    void Route::bind_waypoint(IWaypoint& waypoint)
    {
        waypoint.set_route(shared_from_this());
        waypoint.set_route_colour(_colour);
        waypoint.set_waypoint_colour(_waypoint_colour);
        waypoint.on_changed += on_changed;
    }

    void Route::unbind_waypoint(IWaypoint& waypoint)
    {
        waypoint.set_route({});
        waypoint.on_changed -= on_changed;
    }

    void Route::import(const std::vector<uint8_t>& data)
    {
        Colour new_route_colour = colour();
        Colour new_waypoint_colour = waypoint_colour();

        auto json = nlohmann::json::parse(data.begin(), data.end());
        if (json["colour"].is_string())
        {
            new_route_colour = from_colour_code(json["colour"].get<std::string>());
        }

        if (json["waypoint_colour"].is_string())
        {
            new_waypoint_colour = from_colour_code(json["waypoint_colour"].get<std::string>());
        }

        std::vector<std::shared_ptr<IWaypoint>> new_waypoints;
        for (const auto& waypoint : json["waypoints"])
        {
            auto type_string = waypoint["type"].get<std::string>();
            IWaypoint::Type type = waypoint_type_from_string(type_string);
            Vector3 position = load_vector3(waypoint, "position", Vector3::Zero);
            Vector3 normal = load_vector3(waypoint, "normal", Vector3::Down);

            auto room = waypoint["room"].get<int>();
            auto index = waypoint["index"].get<int>();
            auto notes = waypoint["notes"].get<std::string>();

            auto new_waypoint = _waypoint_source(position, normal, room, type, index, new_route_colour, new_waypoint_colour);
            new_waypoint->set_notes(notes);
            new_waypoint->set_save_file(from_base64(waypoint.value("save", "")));
            new_waypoints.push_back(new_waypoint);
        }

        set_colour(new_waypoint_colour);
        set_waypoint_colour(new_waypoint_colour);
        _waypoints = new_waypoints;
    }

    std::shared_ptr<IRoute> import_route(const IRoute::Source& route_source, const std::shared_ptr<IFiles>& files, const std::string& route_filename)
    {
        try
        {
            auto data = files->load_file(route_filename);
            if (!data.has_value())
            {
                return nullptr;
            }
            return import_trview_route(route_source, data.value());
        }
        catch (std::exception& e)
        {
            MessageBoxA(0, e.what(), "Error", MB_OK);
            return nullptr;
        }
    }
}
