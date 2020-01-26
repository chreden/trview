#pragma once

#include <trview.graphics/Device.h>
#include <trview.app/Geometry/PickResult.h>
#include "ActionNodeOption.h"
#include <array>

namespace trview
{
    struct ICamera;
    class TransparencyBuffer;

    class ActionMenu
    {
    public:
        ActionMenu();

        void render(const graphics::Device& device,
            const ICamera& camera,
            TransparencyBuffer& transparency_buffer,
            const DirectX::SimpleMath::Vector3& previous_waypoint,
            const DirectX::SimpleMath::Vector3& next_waypoint,
            bool selected);

        void select();

        void update(float elapsed);

        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;
    private:
        void generate_geometry();

        std::unique_ptr<ActionNodeOption> _action;
        float _time { 0.0f };
        std::array<DirectX::SimpleMath::Vector3, 8> _sub_nodes;
    };
}
