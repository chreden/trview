#pragma once

#include "../../Routing/IWaypoint.h"

namespace trview
{
    namespace mocks
    {
        struct MockWaypoint : public IWaypoint
        {
            MockWaypoint();
            virtual ~MockWaypoint();
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(Type, type, (), (const, override));
            MOCK_METHOD(bool, has_save, (), (const, override));
            MOCK_METHOD(uint32_t, index, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IItem>, item, (), (const, override));
            MOCK_METHOD(std::string, notes, (), (const, override));
            MOCK_METHOD(uint32_t, room, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IRoute>, route, (), (const, override));
            MOCK_METHOD(std::vector<uint8_t>, save_file, (), (const, override));
            MOCK_METHOD(void, set_item, (const std::weak_ptr<IItem>&), (override));
            MOCK_METHOD(void, set_normal, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, set_notes, (const std::string&), (override));
            MOCK_METHOD(void, set_route, (const std::weak_ptr<IRoute>&), (override));
            MOCK_METHOD(void, set_route_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_waypoint_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_save_file, (const std::vector<uint8_t>&), (override));
            MOCK_METHOD(void, set_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, render_join, (const IWaypoint&, const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, blob_position, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, normal, (), (const, override));
            MOCK_METHOD(WaypointRandomizerSettings, randomizer_settings, (), (const, override));
            MOCK_METHOD(void, set_randomizer_settings, (const WaypointRandomizerSettings&), (override));
            MOCK_METHOD(void, set_position, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(std::weak_ptr<ITrigger>, trigger, (), (const, override));
            MOCK_METHOD(Colour, route_colour, (), (const, override));
            MOCK_METHOD(Colour, waypoint_colour, (), (const, override));
            /// <summary>
            /// Index used for testing ordering.
            /// </summary>
            uint32_t test_index;
        };
    }
}

