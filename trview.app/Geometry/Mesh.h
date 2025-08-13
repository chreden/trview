#pragma once

#include <unordered_set>
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
        Mesh(const std::shared_ptr<graphics::IDevice>& device, const std::vector<Triangle>& triangles, const std::shared_ptr<ITextureStorage>& texture_storage);

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

        std::vector<Triangle> transparent_triangles() const override;

        void update(float delta) override;

        virtual const DirectX::BoundingBox& bounding_box() const override;

        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
    private:
        void calculate_bounding_box(const std::vector<Triangle>& triangles);
        void generate_matrix_buffer();
        void generate_animated_vertex_buffer();

        struct Indices
        {
            uint32_t count{ 0u };
            Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
        };

        std::shared_ptr<graphics::IDevice> _device;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _vertex_buffer;
        std::unordered_map<uint32_t, Indices>             _index_buffers;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _matrix_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                                          _untextured_index_count{ 0u };
        std::vector<Triangle>                          _transparent_triangles;
        std::vector<Triangle>                          _collision_triangles;
        DirectX::BoundingBox                              _bounding_box;
        std::weak_ptr<ITextureStorage>                    _texture_storage;
        std::vector<Triangle>                          _animated_triangles;
        std::unordered_set<uint32_t>                      _animated_triangle_textures;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _animated_vertex_buffer;
    };
}