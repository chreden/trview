#include "Route.h"
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.common/Strings.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const float PoleThickness = 0.05f;
        const float RopeThickness = 0.015f;

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
    }

    Route::Route(const graphics::Device& device, const graphics::IShaderStorage& shader_storage)
        : _waypoint_mesh(create_cube_mesh(device)), _selection_renderer(device, shader_storage)
    {
    }

    void Route::add(const Vector3& position, uint32_t room)
    {
        add(position, room, Waypoint::Type::Position, 0u);
    }

    void Route::add(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index)
    {
        _waypoints.emplace_back(_waypoint_mesh.get(), position, room, type, type_index, _colour);
    }

    Colour Route::colour() const
    {
        return _colour;
    }

    void Route::clear()
    {
        _waypoints.clear();
        _selected_index = 0u;
    }

    void Route::insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index)
    {
        if (index >= _waypoints.size())
        {
            return add(position, room, Waypoint::Type::Position, 0u);
        }
        insert(position, room, index, Waypoint::Type::Position, 0u);
    }

    uint32_t Route::insert(const DirectX::SimpleMath::Vector3& position, uint32_t room)
    {
        uint32_t index = next_index();
        insert(position, room, index);
        return index;
    }

    void Route::insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index, Waypoint::Type type, uint32_t type_index)
    {
        _waypoints.insert(_waypoints.begin() + index, Waypoint(_waypoint_mesh.get(), position, room, type, type_index, _colour));
    }

    uint32_t Route::insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index)
    {
        uint32_t index = next_index();
        insert(position, room, index, type, type_index);
        return index;
    }

    PickResult Route::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result;
        result.hit = false;

        for (uint32_t i = 0; i < _waypoints.size(); ++i)
        {
            const auto& waypoint = _waypoints[i];

            auto box = BoundingBox(waypoint.position() - Vector3(0, 0.25f, 0), Vector3(PoleThickness, 0.5f, PoleThickness) * 0.5f);

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
    }

    void Route::render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        for (std::size_t i = 0; i < _waypoints.size(); ++i)
        {
            auto& waypoint = _waypoints[i];
            waypoint.render(device, camera, texture_storage, Color(1.0f, 1.0f, 1.0f));

            // Should render the in-between line somehow - if there is another point in the list.
            if (i < _waypoints.size() - 1)
            {
                /*
                const auto current = waypoint.position() - Vector3(0, 0.5f + PoleThickness * 0.5f, 0);
                const auto next_waypoint = _waypoints[i + 1].position() - Vector3(0, 0.5f + PoleThickness * 0.5f, 0);
                const auto mid = Vector3::Lerp(current, next_waypoint, 0.5f);
                const auto matrix = Matrix(DirectX::XMMatrixLookAtRH(mid, next_waypoint, Vector3::Up)).Invert();
                const auto length = (next_waypoint - current).Length();
                const auto to_wvp = Matrix::CreateScale(RopeThickness, RopeThickness, length) * matrix * camera.view_projection();
                _waypoint_mesh->render(device.context(), to_wvp, texture_storage, _colour);
                */
            }
        }

        // Render selected waypoint...
        if (_selected_index < _waypoints.size())
        {
            _selection_renderer.render(device, camera, texture_storage, _waypoints[_selected_index], Color(1.0f, 1.0f, 1.0f));
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
            waypoint.set_route_colour(colour);
        }
    }

    const Waypoint& Route::waypoint(uint32_t index) const
    {
        if (index < _waypoints.size())
        {
            return _waypoints[index];
        }
        throw std::range_error("Waypoint index out of range");
    }

    Waypoint& Route::waypoint(uint32_t index)
    {
        if (index < _waypoints.size())
        {
            return _waypoints[index];
        }
        throw std::range_error("Waypoint index out of range");
    }

    uint32_t Route::waypoints() const
    {
        return _waypoints.size();
    }

    uint32_t Route::next_index() const
    {
        return _waypoints.empty() ? 0 : _selected_index + 1;
    }

    std::unique_ptr<Route> import_route(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const std::string& filename)
    {
        try
        {
            std::ifstream file(to_utf16(filename));
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            if (!file.is_open())
            {
                return std::unique_ptr<Route>();
            }

            auto route = std::make_unique<Route>(device, shader_storage);

            nlohmann::json json;
            file >> json;

            if (json["colour"].is_string())
            {
                route->set_colour(named_colour(to_utf16(json["colour"].get<std::string>())));
            }

            for (const auto& waypoint : json["waypoints"])
            {
                auto type_string = waypoint["type"].get<std::string>();
                Waypoint::Type type = waypoint_type_from_string(type_string);
 
                auto position_string = waypoint["position"].get<std::string>();

                std::stringstream stringstream(position_string);

                std::vector<float> result;
                for (int i = 0; i < 3; ++i)
                {
                    std::string substr;
                    std::getline(stringstream, substr, ',');
                    result.push_back(std::stof(substr));
                }

                Vector3 position = Vector3(result[0], result[1], result[2]);

                auto room = waypoint["room"].get<int>();
                auto index = waypoint["index"].get<int>();
                auto notes = waypoint["notes"].get<std::string>();

                route->add(position, room, type, index);

                auto& new_waypoint = route->waypoint(route->waypoints() - 1);
                new_waypoint.set_notes(to_utf16(notes));
                new_waypoint.set_save_file(from_base64(waypoint.value("save", "")));
            }

            return route;
        }
        catch (std::exception&)
        {
            return std::unique_ptr<Route>();
        }
    }

    void export_route(const Route& route, const std::string& filename)
    {
        try
        {
            nlohmann::json json;

            json["colour"] = to_utf8(route.colour().name());

            std::vector<nlohmann::json> waypoints;

            for (uint32_t i = 0; i < route.waypoints(); ++i)
            {
                const Waypoint& waypoint = route.waypoint(i);
                nlohmann::json waypoint_json;
                waypoint_json["type"] = to_utf8(waypoint_type_to_string(waypoint.type()));

                std::stringstream pos_string;
                auto pos = waypoint.position();
                pos_string << pos.x << "," << pos.y << "," << pos.z;
                waypoint_json["position"] = pos_string.str();
                waypoint_json["room"] = waypoint.room();
                waypoint_json["index"] = waypoint.index();
                waypoint_json["notes"] = to_utf8(waypoint.notes());

                if (waypoint.has_save())
                {
                    waypoint_json["save"] = to_base64(waypoint.save_file());
                }

                waypoints.push_back(waypoint_json);
            }

            json["waypoints"] = waypoints;

            std::ofstream file(to_utf16(filename));
            file << json;
        }
        catch (...)
        {
        }
    }
}
