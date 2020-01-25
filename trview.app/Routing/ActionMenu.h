#pragma once

#include <trview.graphics/Device.h>
#include "ActionNodeOption.h"

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
    private:
        void generate_geometry();

        std::unique_ptr<ActionNodeOption> _action;
        float _time { 0.0f };
    };
}
