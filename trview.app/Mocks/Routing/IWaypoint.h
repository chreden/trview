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
            MOCK_METHOD(std::string, notes, (), (const, override));
            MOCK_METHOD(uint32_t, room, (), (const, override));
            MOCK_METHOD(std::vector<uint8_t>, save_file, (), (const, override));
            MOCK_METHOD(void, set_notes, (const std::string&), (override));
            MOCK_METHOD(void, set_route_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_waypoint_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_save_file, (const std::vector<uint8_t>&), (override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, render_join, (const IWaypoint&, const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, blob_position, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, normal, (), (const, override));
            MOCK_METHOD(WaypointRandomizerSettings, randomizer_settings, (), (const, override));
            MOCK_METHOD(void, set_randomizer_settings, (const WaypointRandomizerSettings&), (override));
            /// <summary>
            /// Index used for testing ordering.
            /// </summary>
            uint32_t test_index;
        };
    }
}

