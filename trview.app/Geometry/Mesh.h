#pragma once

#include <memory>
#include <trlevel/trtypes.h>
#include <trlevel/LevelVersion.h>
#include <trview.graphics/IDevice.h>
#include "IMesh.h"

namespace trview
{
    struct ILevelTextureStorage;

    class Mesh : public IMesh
    {
    public:
        /// Create a mesh using the specified vertices and indices.
        /// @param device The D3D device to create the mesh.
        /// @param vertices The vertices that make up the mesh.
        /// @param indices The indices for triangles that use level textures.
        /// @param untextured_indices The indices for triangles that do not use level textures.
        /// @param transparent_triangles The transparent triangles to use to create the mesh.
        /// @param collision_triangles The triangles for picking.
        Mesh(const std::shared_ptr<graphics::IDevice>& device,
             const std::vector<MeshVertex>& vertices, 
             const std::vector<std::vector<uint32_t>>& indices, 
             const std::vector<uint32_t>& untextured_indices,
             const std::vector<TransparentTriangle>& transparent_triangles,
             const std::vector<Triangle>& collision_triangles,
             const std::vector<AnimatedTriangle>& animated_triangles,
             const std::shared_ptr<ITextureStorage>& texture_storage);

        /// Create a mesh using the specified vertices and indices.
        /// @param transparent_triangles The triangles to use to create the mesh.
        /// @param collision_triangles The triangles for picking.
        Mesh(const std::vector<TransparentTriangle>& transparent_triangles, const std::vector<Triangle>& collision_triangles);

        virtual ~Mesh() = default;

        virtual void render(const DirectX::SimpleMath::Matrix& world_view_projection,
            const DirectX::SimpleMath::Color& colour,
            float light_intensity = 1.0f,
            DirectX::SimpleMath::Vector3 light_direction = DirectX::SimpleMath::Vector3::Zero,
            bool geometry_mode = false,
            bool use_colour_override = false) override;

        virtual void render(const DirectX::SimpleMath::Matrix& world_view_projection,
            const graphics::Texture& replacement_texture,
            const DirectX::SimpleMath::Color& colour,
            float light_intensity = 1.0f,
            DirectX::SimpleMath::Vector3 light_direction = DirectX::SimpleMath::Vector3::Zero) override;

        virtual std::vector<TransparentTriangle> transparent_triangles() const override;

        virtual const DirectX::BoundingBox& bounding_box() const override;

        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
    private:
        void calculate_bounding_box(const std::vector<MeshVertex>& vertices, const std::vector<TransparentTriangle>& transparent_triangles);
        void generate_animated_vertex_buffer();

        std::shared_ptr<graphics::IDevice> _device;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _vertex_buffer;
        std::vector<uint32_t>                             _index_counts;
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> _index_buffers;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _matrix_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                                          _untextured_index_count{ 0u };
        std::vector<TransparentTriangle>                  _transparent_triangles;
        std::vector<Triangle>                             _collision_triangles;
        DirectX::BoundingBox                              _bounding_box;
        std::weak_ptr<ITextureStorage>                    _texture_storage;
        std::vector<AnimatedTriangle>                     _animated_triangles;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _animated_vertex_buffer;
    };
}