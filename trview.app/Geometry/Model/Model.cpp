#include "Model.h"
#include "../ITransparencyBuffer.h"

namespace trview
{
    namespace
    {
        bool is_null_model(const std::vector<std::shared_ptr<IMesh>>& meshes)
        {
            if (meshes.size() != 1)
            {
                return false;
            }

            const auto triangles = meshes[0]->triangles();
            if (triangles.empty() ||
                triangles.size() == 1 && triangles[0].transparency_mode != Triangle::TransparencyMode::None)
            {
                return true;
            }

            return false;
        }
    }

    IModel::~IModel()
    {
    }

    Model::Model(const trlevel::tr_model& model, const std::vector<std::shared_ptr<IMesh>>& meshes, const std::vector<DirectX::SimpleMath::Matrix>& transforms, const std::weak_ptr<IMesh>& null_mesh, const std::weak_ptr<ITextureStorage>& texture_storage)
        : _meshes(meshes), _world_transforms(transforms), _model(model)
    {
        check_for_null_model(null_mesh, texture_storage);
        generate_bounding_box();
    }

    DirectX::BoundingBox Model::bounding_box() const
    {
        return _bounding_box;
    }

    PickResult Model::pick(const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        using namespace DirectX;
        using namespace DirectX::SimpleMath;

        std::vector<uint32_t> pick_meshes;

        // Check each of the meshes in the object.
        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            // Transform the position and the direction into mesh space.
            const auto transform = world.Invert();
            const auto transformed_position = Vector3::Transform(position, transform);
            const auto transformed_direction = Vector3::TransformNormal(direction, transform);

            // Try and pick against the bounding box.
            float obb_distance = 0;
            if (_oriented_boxes[i].Intersects(transformed_position, transformed_direction, obb_distance))
            {
                // Pick against the triangles in this mesh.
                pick_meshes.push_back(i);
            }
        }

        if (pick_meshes.empty())
        {
            return PickResult();
        }

        PickResult result;
        result.type = PickResult::Type::Entity;

        for (auto i : pick_meshes)
        {
            // Transform the position and the direction into mesh space.
            const auto transform = (_world_transforms[i] * world).Invert();
            const auto transformed_position = Vector3::Transform(position, transform);
            const auto transformed_direction = Vector3::TransformNormal(direction, transform);

            // Pick against mesh.
            auto mesh_result = _meshes[i]->pick(transformed_position, transformed_direction);
            if (mesh_result.hit && mesh_result.distance < result.distance)
            {
                result.hit = true;
                result.distance = mesh_result.distance;
                result.position = position + direction * mesh_result.distance;
            }
        }

        return result;
    }

    void Model::render(const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view_projection, const DirectX::SimpleMath::Color& colour)
    {
        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            const auto wvp = _world_transforms[i] * world * view_projection;
            if (_null_texture.has_value())
            {
                _meshes[i]->render(wvp, _null_texture.value(), colour);
            }
            else
            {
                _meshes[i]->render(wvp, colour);
            }
        }
    }

    void Model::render_transparency(const DirectX::SimpleMath::Matrix& world, ITransparencyBuffer& transparency, const DirectX::SimpleMath::Color& colour)
    {
        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            for (const auto& triangle : _meshes[i]->transparent_triangles())
            {
                transparency.add(triangle.transform(_world_transforms[i] * world, colour, true));
            }
        }
    }

    uint32_t Model::type_id() const
    {
        return _model.ID;
    }

    void Model::generate_bounding_box()
    {
        using namespace DirectX;
        using namespace DirectX::SimpleMath;

        // Remove any stored boxes so they can be created.
        _oriented_boxes.clear();

        // The entity bounding box is based on the bounding boxes of the meshes it contains.
        // Allocate space for all of the corners.
        std::vector<Vector3> corners(_meshes.size() * 8);

        for (uint32_t i = 0; i < _meshes.size(); ++i)
        {
            // Get the box for the mesh.
            auto box = _meshes[i]->bounding_box();

            // Transform the box by the model transform. Store this box for later picking.
            BoundingOrientedBox oriented_box;
            BoundingOrientedBox::CreateFromBoundingBox(oriented_box, box);
            oriented_box.Transform(oriented_box, _world_transforms[i]);
            oriented_box.GetCorners(&corners[i * 8]);
            _oriented_boxes.push_back(oriented_box);
        }

        if (!corners.empty())
        {
            // Create an axis-aligned BB from the points of the oriented ones.
            BoundingBox::CreateFromPoints(_bounding_box, corners.size(), &corners[0], sizeof(Vector3));
        }
    }

    void Model::check_for_null_model(const std::weak_ptr<IMesh>& null_mesh, const std::weak_ptr<ITextureStorage>& texture_storage)
    {
        if (is_null_model(_meshes))
        {
            _meshes = { null_mesh.lock() };
            _world_transforms = { DirectX::SimpleMath::Matrix::CreateScale(0.1f) * _world_transforms[0] };
            if (auto ts = texture_storage.lock())
            {
                _null_texture = ts->lookup("placeholder_texture");
            }
        }
    }
}

