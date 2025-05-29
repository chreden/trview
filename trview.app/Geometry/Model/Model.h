#pragma once

#include "../IMesh.h"
#include "IModel.h"

namespace trview
{
    class Model final : public IModel
    {
    public:
        explicit Model(const trlevel::tr_model& model, const std::vector<std::shared_ptr<IMesh>>& meshes, const std::vector<DirectX::SimpleMath::Matrix>& transforms);
        virtual ~Model() = default;
        DirectX::BoundingBox bounding_box() const override;
        PickResult pick(const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        void render(const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view_projection, const DirectX::SimpleMath::Color& colour) override;
        void render_transparency(const DirectX::SimpleMath::Matrix& world, ITransparencyBuffer& transparency, const DirectX::SimpleMath::Color& colour) override;
        uint32_t type_id() const override;
    private:
        void generate_bounding_box();

        std::vector<std::shared_ptr<IMesh>>       _meshes;
        std::vector<DirectX::SimpleMath::Matrix>  _world_transforms;
        DirectX::BoundingBox                      _bounding_box;
        std::vector<DirectX::BoundingOrientedBox> _oriented_boxes;
        trlevel::tr_model                         _model;
    };
}

