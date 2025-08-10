#pragma once

#include "AnimatedTriangle.h"
#include "TransparentTriangle.h"
#include "MeshVertex.h"
#include "Triangle.h"
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Geometry/PickResult.h>

namespace trview
{
    struct IMesh
    {
        using Source = std::function<std::shared_ptr<IMesh>(const std::vector<UniTriangle>&)>;

        virtual ~IMesh() = 0;

        virtual void render(const DirectX::SimpleMath::Matrix& world_view_projection,
            const DirectX::SimpleMath::Color& colour,
            float light_intensity = 1.0f,
            DirectX::SimpleMath::Vector3 light_direction = DirectX::SimpleMath::Vector3::Zero,
            bool geometry_mode = false,
            bool use_colour_override = false) = 0;

        virtual void render(const DirectX::SimpleMath::Matrix& world_view_projection,
            const graphics::Texture& replacement_texture,
            const DirectX::SimpleMath::Color& colour,
            float light_intensity = 1.0f,
            DirectX::SimpleMath::Vector3 light_direction = DirectX::SimpleMath::Vector3::Zero) = 0;

        virtual std::vector<TransparentTriangle> transparent_triangles() const = 0;

        virtual void update(float delta) = 0;

        virtual const DirectX::BoundingBox& bounding_box() const = 0;

        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
    };

    /// Create a new mesh based on the contents of the mesh specified.
    /// @param mesh The level mesh to generate.
    /// @param source The mesh source function.
    /// @param texture_storage The textures for the level.
    /// @param transparent_collision Whether to include transparent triangles in collision triangles.
    /// @returns The new mesh.
    std::shared_ptr<IMesh> create_mesh(const trlevel::tr_mesh& mesh, const IMesh::Source& source, const ILevelTextureStorage& texture_storage, const trlevel::PlatformAndVersion& platform_and_version, bool transparent_collision = true);

    /// Create a new cube mesh.
    std::shared_ptr<IMesh> create_cube_mesh(const IMesh::Source& source);
    std::shared_ptr<IMesh> create_frustum_mesh(const IMesh::Source& source);

    std::shared_ptr<IMesh> create_sphere_mesh(const IMesh::Source& source, uint32_t stacks, uint32_t slices);

    enum class SpriteOffsetMode
    {
        RoomSprite,
        Entity
    };

    /// Create a new sprite mesh.
    /// @param source The function to call to create a IMesh
    /// @param sprite The game sprite definition to use.
    /// @param scale The output scale matrix.
    /// @param offset The output offset.
    std::shared_ptr<IMesh> create_sprite_mesh(
        const IMesh::Source& source,
        const std::optional<trlevel::tr_sprite_texture>& sprite,
        DirectX::SimpleMath::Matrix& scale,
        DirectX::SimpleMath::Vector3& offset,
        SpriteOffsetMode offset_mode);

    /// Create a new sprite mesh.
    /// @param source The function to call to create a IMesh
    /// @param sprite The game sprite definition to use.
    /// @param scale The output scale matrix.
    std::shared_ptr<IMesh> create_sprite_mesh(
        const IMesh::Source& source,
        const std::optional<trlevel::tr_sprite_texture>& sprite,
        DirectX::SimpleMath::Matrix& scale,
        DirectX::SimpleMath::Matrix& offset,
        SpriteOffsetMode offset_mode);

    /// Convert the textured rectangles into collections required to create a mesh.
    /// @param rectangles The rectangles from the mesh or room geometry.
    /// @param input_vertices The vertices that the rectangle indices refer to.
    /// @param texture_storage The texture storage for the level.
    /// @param output_vertices The collection to add the new vertices to.
    /// @param output_indices The collection to add new indices to.
    /// @param transparent_triangles The collection to add transparent triangles to.
    /// @param collision_triangles The collection to add collision triangles to.
    /// @param transparent_collision Whether to add transparent rectangles as collision triangles.
    void process_textured_rectangles(
        const std::vector<trlevel::tr4_mesh_face4>& rectangles,
        const std::vector<trlevel::trview_room_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<UniTriangle>& out_triangles,
        bool transparent_collision);

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
        const std::vector<trlevel::tr4_mesh_face3>& triangles,
        const std::vector<trlevel::trview_room_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<UniTriangle>& out_triangles,
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
        const std::vector<trlevel::trview_room_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<UniTriangle>& out_triangles,
        const trlevel::PlatformAndVersion& platform_and_version);

    // Convert the coloured triangles into collections required to create a mesh.
    // triangles: The triangles from the mesh or room geometry.
    // input_vertices: The vertices that the triangle indices refer to.
    // texture_storage: The texture storage for the level.
    // output_vertices: The collection to add the new vertices to.
    // output_indices: The collection to add new indices to.
    // collision_triangles: The collection to add collision triangles to.
    void process_coloured_triangles(
        const std::vector<trlevel::tr_face3>& triangles,
        const std::vector<trlevel::trview_room_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<UniTriangle>& out_triangles,
        const trlevel::PlatformAndVersion& platform_and_version);

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
        float light_intensity{ 1.0f };
        int light_enabled{ 0 };
        int colour_override_enabled{ 0 };
        DirectX::SimpleMath::Vector4 colour_override { 1, 1, 1, 1 };
    };
#pragma warning(pop)
}
