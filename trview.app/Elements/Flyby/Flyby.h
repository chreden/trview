#pragma once

#include "IFlyby.h"
#include "../../Geometry/IMesh.h"

namespace trview
{
    class Flyby final : public IFlyby
    {
    public:
        explicit Flyby(
            const IMesh::Source& mesh_source,
            const std::shared_ptr<IMesh>& mesh,
            const std::vector<trlevel::tr4_flyby_camera>& camera_nodes, 
            uint32_t index);
        virtual ~Flyby() = default;
        std::vector<trlevel::tr4_flyby_camera> nodes() const override;
        uint32_t number() const override;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        bool visible() const override;
        void set_visible(bool value) override;
        [[nodiscard]] CameraState update_state(const CameraState& state, float delta) const override;
    private:
        void generate_path(const IMesh::Source& mesh_source);
        void state_at(CameraState& state) const;

        uint32_t _index{ 0u };
        std::vector<trlevel::tr4_flyby_camera> _camera_nodes;
        std::shared_ptr<IMesh> _mesh;
        bool _visible{ true };
        DirectX::SimpleMath::Color _colour;
        std::vector<DirectX::SimpleMath::Vector3> _temp;
        std::shared_ptr<IMesh> _path_mesh;
    };
}

