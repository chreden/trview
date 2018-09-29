#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <memory>

#include <trlevel/trtypes.h>

#include "MeshVertex.h"
#include "TransparencyBuffer.h"
#include "Triangle.h"

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
        /// @param collision_triangles The triangles for picking.
        Mesh(const Microsoft::WRL::ComPtr<ID3D11Device>& device,
             const std::vector<MeshVertex>& vertices, 
             const std::vector<std::vector<uint32_t>>& indices, 
             const std::vector<uint32_t>& untextured_indices,
             const std::vector<TransparentTriangle>& transparent_triangles,
             const std::vector<Triangle>& collision_triangles);

        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const DirectX::SimpleMath::Matrix& world_view_projection, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour);

        const std::vector<TransparentTriangle>& transparent_triangles() const;

        const DirectX::BoundingBox& bounding_box() const;
    private:
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

    // Create a new mesh based on the contents of the mesh specified.
    // mesh: The level mesh to generate.
    // device: The D3D device to use to create the mesh.
    // texture_storage: The textures for the level.
    // Returns: The new mesh.
    std::unique_ptr<Mesh> create_mesh(const trlevel::tr_mesh& mesh, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const ILevelTextureStorage& texture_storage);

    // Convert the textured rectangles into collections required to create a mesh.
    // rectangles: The rectangles from the mesh or room geometry.
    // input_vertices: The vertices that the rectangle indices refer to.
    // texture_storage: The texture storage for the level.
    // output_vertices: The collection to add the new vertices to.
    // output_indices: The collection to add new indices to.
    // transparent_triangles: The collection to add transparent triangles to.
    // collision_triangles: The collection to add collision triangles to.
    void process_textured_rectangles(
        const std::vector<trlevel::tr4_mesh_face4>& rectangles, 
        const std::vector<trlevel::tr_vertex>& input_vertices, 
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles);

    // Convert the textured triangles into collections required to create a mesh.
    // triangles: The triangles from the mesh or room geometry.
    // input_vertices: The vertices that the triangle indices refer to.
    // texture_storage: The texture storage for the level.
    // output_vertices: The collection to add the new vertices to.
    // output_indices: The collection to add new indices to.
    // transparent_triangles: The collection to add transparent triangles to.
    // collision_triangles: The collection to add collision triangles to.
    void process_textured_triangles(
        const std::vector<trlevel::tr4_mesh_face3>& triangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles);

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
}