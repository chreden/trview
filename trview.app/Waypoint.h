#pragma once

#include <SimpleMath.h>
#include "IRenderable.h"
#include "Mesh.h"

namespace trview
{
    class Waypoint final : public IRenderable
    {
    public:
        explicit Waypoint(Mesh* mesh, const DirectX::SimpleMath::Vector3& position);
        virtual ~Waypoint() = default;
        virtual void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        DirectX::SimpleMath::Vector3 position() const;
    private:
        std::string                  _name;
        DirectX::SimpleMath::Vector3 _position;
        Mesh*                        _mesh;
    };
}
