#pragma once

#include "IFlyby.h"
#include "../../Geometry/IMesh.h"

namespace trview
{
    class Flyby final : public IFlyby
    {
    public:
        explicit Flyby(const std::shared_ptr<IMesh>& mesh, const std::vector<trlevel::tr4_flyby_camera>& camera_nodes);
        virtual ~Flyby() = default;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        bool visible() const override;
        void set_visible(bool value) override;
    private:
        std::vector<trlevel::tr4_flyby_camera> _camera_nodes;
        std::shared_ptr<IMesh> _mesh;
        bool _visible{ true };
        DirectX::SimpleMath::Color _colour;
    };
}

