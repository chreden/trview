#pragma once

#include "IMover.h"
#include <trview.app/Geometry/IMesh.h>

namespace trview
{
    class Mover final : public IMover
    {
    public:
        explicit Mover(const IMesh::Source& mesh_source);
        virtual ~Mover() = default;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) override;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) override;
    private:
        std::shared_ptr<IMesh> _mesh;
        DirectX::SimpleMath::Vector3 _position;
    };
}
