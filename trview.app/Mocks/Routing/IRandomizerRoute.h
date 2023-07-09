#pragma once

#include "../../Routing/IRandomizerRoute.h"

namespace trview
{
    namespace mocks
    {
        struct MockRandomizerRoute : public IRandomizerRoute
        {
            MockRandomizerRoute();
            virtual ~MockRandomizerRoute();
            MOCK_METHOD(std::shared_ptr<IWaypoint>, add, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t), (override));
            MOCK_METHOD(std::shared_ptr<IWaypoint>, add, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, IWaypoint::Type, uint32_t), (override));
            MOCK_METHOD(std::shared_ptr<IWaypoint>, add, (const std::shared_ptr<IWaypoint>&), (override));
            MOCK_METHOD(std::shared_ptr<IWaypoint>, add, (const std::string&, const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t), (override));
            MOCK_METHOD(void, clear, (), (override));
            MOCK_METHOD(Colour, colour, (), (const, override));
            MOCK_METHOD(std::optional<std::string>, filename, (), (const, override));
            MOCK_METHOD(std::vector<std::string>, filenames, (), (const, override));
            MOCK_METHOD(void, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, uint32_t), (override));
            MOCK_METHOD(uint32_t, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t), (override));
            MOCK_METHOD(void, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, uint32_t, IWaypoint::Type, uint32_t), (override));
            MOCK_METHOD(uint32_t, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, IWaypoint::Type, uint32_t), (override));
            MOCK_METHOD(bool, is_unsaved, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
            MOCK_METHOD(void, move, (int32_t, int32_t), (override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(void, remove, (uint32_t), (override));
            MOCK_METHOD(void, remove, (const std::shared_ptr<IWaypoint>&), (override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, bool), (override));
            MOCK_METHOD(void, save, (const std::shared_ptr<IFiles>&, const UserSettings&), (override));
            MOCK_METHOD(uint32_t, selected_waypoint, (), (const, override));
            MOCK_METHOD(void, select_waypoint, (uint32_t), (override));
            MOCK_METHOD(void, set_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_filename, (const std::string&), (override));
            MOCK_METHOD(void, set_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(void, set_randomizer_enabled, (bool), (override));
            MOCK_METHOD(void, set_unsaved, (bool), (override));
            MOCK_METHOD(void, set_waypoint_colour, (const Colour&), (override));
            MOCK_METHOD(Colour, waypoint_colour, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IWaypoint>, waypoint, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, waypoints, (), (const, override));
        };
    }
}
