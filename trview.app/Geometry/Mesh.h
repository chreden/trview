#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <memory>

#include <trlevel/trtypes.h>
#include <trlevel/LevelVersion.h>
#include <trview.graphics/Device.h>

#include "MeshVertex.h"
#include "TransparentTriangle.h"
#include "Triangle.h"
#include <trview.app/Geometry/PickResult.h>

namespace trview
{
    struct ILevelTextureStorage;

    class Mesh
    {
    public:
        /// Create a mesh using the specified vertices and indices.
        /// @param device The D3D device to create the mesh.
        /// @param vertices The vertices that make up the mesh.
        /// @param indices The indices for triangles that use level textures.
        /// @param untextured_indices The indices for triangles that do not use level textures.
        /// @param transparent_triangles The transparent triangles to use to create the mesh.
        /// @param collision_triangles The triangles for picking.
        Mesh(const graphics::Device& device,
             const std::vector<MeshVertex>& vertices, 
             const std::vector<std::vector<uint32_t>>& indices, 
             const std::vector<uint32_t>& untextured_indices,
             const std::vector<TransparentTriangle>& transparent_triangles,
             const std::vector<Triangle>& collision_triangles);

        /// Create a mesh using the specified vertices and indices.
        /// @param transparent_triangles The triangles to use to create the mesh.
        /// @param collision_triangles The triangles for picking.
        Mesh(const std::vector<TransparentTriangle>& transparent_triangles, const std::vector<Triangle>& collision_triangles);

        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
            const DirectX::SimpleMath::Matrix& world_view_projection,
            const ILevelTextureStorage& texture_storage,
            const DirectX::SimpleMath::Color& colour,
            DirectX::SimpleMath::Vector3 light_direction = DirectX::SimpleMath::Vector3::Zero);

        const std::vector<TransparentTriangle>& transparent_triangles() const;

        const DirectX::BoundingBox& bounding_box() const;

        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;
    private:
        void calculate_bounding_box(const std::vector<MeshVertex>& vertices, const std::vector<TransparentTriangle>& transparent_triangles);

        Microsoft::WRL::ComPtr<ID3D11Buffer>              _vertex_buffer;
        std::vector<uint32_t>                             _index_counts;
        std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> _index_buffers;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _matrix_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>              _untextured_index_buffer;
        uint32_t                                          _untextured_index_count;
        std::vector<TransparentTriangle>                  _transparent_triangles;
        std::vector<Triangle>                             _collision_triangles;
        DirectX::BoundingBox                              _bounding_box;
    };

    /// Create a new mesh based on the contents of the mesh specified.
    /// @param level_version The level version - affects texture index
    /// @param mesh The level mesh to generate.
    /// @param device The D3D device to use to create the mesh.
    /// @param texture_storage The textures for the level.
    /// @param transparent_collision Whether to include transparent triangles in collision triangles.
    /// @returns The new mesh.
    std::unique_ptr<Mesh> create_mesh(trlevel::LevelVersion level_version, const trlevel::tr_mesh& mesh, const graphics::Device& device, const ILevelTextureStorage& texture_storage, bool transparent_collision = true);

    /// Create a new cube mesh.
    std::unique_ptr<Mesh> create_cube_mesh(const graphics::Device& device);

    /// Convert the textured rectangles into collections required to create a mesh.
    /// @param level_version The level version - affects texture index.
    /// @param rectangles The rectangles from the mesh or room geometry.
    /// @param input_vertices The vertices that the rectangle indices refer to.
    /// @param texture_storage The texture storage for the level.
    /// @param output_vertices The collection to add the new vertices to.
    /// @param output_indices The collection to add new indices to.
    /// @param transparent_triangles The collection to add transparent triangles to.
    /// @param collision_triangles The collection to add collision triangles to.
    /// @param transparent_collision Whether to add transparent rectangles as collision triangles.
    void process_textured_rectangles(
        trlevel::LevelVersion level_version,
        const std::vector<trlevel::tr4_mesh_face4>& rectangles, 
        const std::vector<trlevel::tr_vertex>& input_vertices, 
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles,
        bool transparent_collision = true);

    /// Convert the textured triangles into collections required to create a mesh.
    /// @param triangles The triangles from the mesh or room geometry.
    /// @param input_vertices The vertices that the triangle indices refer to.
    /// @param texture_storage The texture storage for the level.
    /// @param output_vertices The collection to add the new vertices to.
    /// @param output_indices The collection to add new indices to.
    /// @param transparent_triangles The collection to add transparent triangles to.
    /// @param collision_triangles The collection to add collision triangles to.
    /// @param transparent_collision Whether to add transparent rectangles as collision triangles.
    void process_textured_triangles(
        trlevel::LevelVersion level_version,
        const std::vector<trlevel::tr4_mesh_face3>& triangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles,
        bool transparent_collision = true);

    // Convert the coloured rectangles into collections required to create a mesh.
    // triangles: The rectangles from the mesh or room geometry.
    // input_vertices: The vertices that the rectangle indices refer to.
    // texture_storage: The texture storage for the level.
    // output_vertices: The collection to add the new vertices to.
    // output_indices: The collection to add new indices to.
    // collision_triangles: The collection to add collision triangles to.
    void process_coloured_rectangles(
        const std::vector<trlevel::tr_face4>& rectangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles);

    // Convert the coloured triangles into collections required to create a mesh.
    // triangles: The triangles from the mesh or room geometry.
    // input_vertices: The vertices that the triangle indices refer to.
    // texture_storage: The texture storage for the level.
    // output_vertices: The collection to add the new vertices to.
    // output_indices: The collection to add new indices to.
    // collision_triangles: The collection to add collision triangles to.
    void process_coloured_triangles(
        const std::vector<trlevel::tr_face3>& triangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles);

    /// Convert the vertex to the scale used by the viewer.
    /// @param vertex The vertex to convert.
    /// @retrurns The scaled vector.
    DirectX::SimpleMath::Vector3 convert_vertex(const trlevel::tr_vertex& vertex);

#pragma warning(push)
#pragma warning(disable : 4324)
    __declspec(align(16)) struct MeshData
    {
        DirectX::SimpleMath::Matrix matrix;
        DirectX::SimpleMath::Color colour;
        DirectX::SimpleMath::Vector4 light_dir;
        int light_enabled{ 0 };
    };
#pragma warning(pop)
}