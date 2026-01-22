#include "ModelStorage.h"
#include "../../Graphics/IMeshStorage.h"
#include <ranges>

namespace trview
{
    namespace
    {
        constexpr int16_t LaraSkinTR3 = 315;
        constexpr int16_t LaraSkinTR3Demo55 = 275;
        constexpr int16_t LaraSkinPostTR3 = 8;
        constexpr int16_t LaraSkinTR4OPSM90 = 10;

        int16_t get_skin_id(const trlevel::PlatformAndVersion& version, int16_t type)
        {
            using namespace trlevel;
            if (type != 0 || version.version < LevelVersion::Tomb3)
            {
                return type;
            }
            else if (version.version > trlevel::LevelVersion::Tomb3 || is_tr4_version_111(version))
            {
                if (is_tr4_version_120(version) || is_tr4_version_121(version) || is_tr4_version_124(version) || is_tr4_version_126(version))
                {
                    return LaraSkinTR4OPSM90;
                }
                return LaraSkinPostTR3;
            }
            else if (is_tr3_demo_55(version))
            {
                return LaraSkinTR3Demo55;
            }
            return LaraSkinTR3;
        }

        bool is_skin_id(const trlevel::PlatformAndVersion& version, int16_t type)
        {
            using namespace trlevel;
            if (version.version < LevelVersion::Tomb3)
            {
                return false;
            }
            else if (version.version > trlevel::LevelVersion::Tomb3 || is_tr4_version_111(version))
            {
                if (is_tr4_version_120(version) || is_tr4_version_121(version) || is_tr4_version_124(version) || is_tr4_version_126(version))
                {
                    if (type == LaraSkinTR4OPSM90)
                    {
                        return true;
                    }
                }
                else if (type == LaraSkinPostTR3)
                {
                    return true;
                }
            }
            else if (is_tr3_demo_55(version))
            {
                if (type == LaraSkinTR3Demo55)
                {
                    return true;
                }
            }

            if (type == LaraSkinTR3)
            {
                return true;
            }

            return false;
        }

        std::vector<std::shared_ptr<IMesh>> load_meshes(
            const trlevel::tr_model& model,
            const trlevel::PlatformAndVersion& version,
            const std::shared_ptr<IMeshStorage> mesh_storage)
        {
            std::vector<std::shared_ptr<IMesh>> meshes;
            if (version.platform == trlevel::Platform::PSX && equals_any(version.version, trlevel::LevelVersion::Tomb4, trlevel::LevelVersion::Tomb5))
            {
                const uint32_t end_pointer = static_cast<uint32_t>(model.StartingMesh + model.NumMeshes * 2);
                for (uint32_t mesh_pointer = model.StartingMesh; mesh_pointer < end_pointer; mesh_pointer += 2)
                {
                    auto mesh = mesh_storage->mesh(mesh_pointer);
                    if (mesh)
                    {
                        meshes.push_back(mesh);
                    }
                }
            }
            else
            {
                const uint32_t end_pointer = static_cast<uint32_t>(model.StartingMesh + model.NumMeshes);
                for (uint32_t mesh_pointer = model.StartingMesh; mesh_pointer < end_pointer; ++mesh_pointer)
                {
                    auto mesh = mesh_storage->mesh(mesh_pointer);
                    if (mesh)
                    {
                        meshes.push_back(mesh);
                    }
                }
            }
            return meshes;
        }

        std::vector<DirectX::SimpleMath::Matrix> load_transforms(
            const trlevel::tr_model& model,
            const trlevel::ILevel& level)
        {
            using namespace DirectX;
            using namespace DirectX::SimpleMath;

            auto get_rotate = [](const trlevel::tr2_frame_rotation& r)
                {
                    return Matrix::CreateFromYawPitchRoll(r.y, r.x, r.z);
                };

            trlevel::tr_model actual_model = model;
            if (is_skin_id(level.platform_and_version(), static_cast<int16_t>(model.ID)))
            {
                level.get_model_by_id(0, actual_model);
            }

            std::vector<Matrix> transforms;
            if (actual_model.NumMeshes > 0)
            {
                // Load the frames.
                auto frame = level.get_frame(trlevel::has_double_frames(level.platform_and_version()) ? actual_model.FrameOffset : actual_model.FrameOffset / 2, actual_model.NumMeshes);

                // TODO: Safe frame
                if (frame.values.empty())
                {
                    for (int32_t t = 0; t < actual_model.NumMeshes; ++t)
                    {
                        transforms.push_back(Matrix::Identity);
                    }
                    return transforms;
                }

                uint32_t frame_offset = 0;

                auto initial_frame = frame.values[frame_offset++];

                Matrix initial_rotation = get_rotate(initial_frame);
                Matrix initial_frame_offset = Matrix::CreateTranslation(frame.position());

                Matrix previous_world = initial_rotation * initial_frame_offset;
                transforms.push_back(previous_world);

                std::stack<Matrix> world_stack;

                // Build the mesh tree.
                // Request one less node than we have meshes as the first mesh is at the same position as the entity.
                auto mesh_nodes = level.get_meshtree(actual_model.MeshTree, actual_model.NumMeshes - 1);

                for (const auto& node : mesh_nodes)
                {
                    Matrix parent_world = previous_world;

                    if (node.Flags & 0x1)
                    {
                        if (!world_stack.empty())
                        {
                            parent_world = world_stack.top();
                            world_stack.pop();
                        }
                        else
                        {
                            parent_world = Matrix::Identity;
                        }
                    }
                    if (node.Flags & 0x2)
                    {
                        world_stack.push(parent_world);
                    }

                    // Get the rotation from the frames.
                    // Rotations are performed in Y, X, Z order.
                    auto rotation = frame.values[frame_offset++];
                    Matrix rotation_matrix = get_rotate(rotation);
                    Matrix translation_matrix = Matrix::CreateTranslation(node.position());
                    Matrix node_transform = rotation_matrix * translation_matrix * parent_world;

                    transforms.push_back(node_transform);
                    previous_world = node_transform;
                }
            }
            return transforms;
        }
    }

    IModelStorage::~IModelStorage()
    {
    }

    ModelStorage::ModelStorage(const std::shared_ptr<IMeshStorage>& mesh_storage,
        const IModel::Source& model_source,
        const trlevel::ILevel& level)
        : _platform_and_version(level.platform_and_version())
    {
        load_models(mesh_storage, model_source, level);
    }

    void ModelStorage::load_models(const std::shared_ptr<IMeshStorage>& mesh_storage, const IModel::Source& model_source, const trlevel::ILevel& level)
    {

        using namespace DirectX::SimpleMath;

        const auto version = level.platform_and_version();
        for (uint32_t i = 0; i < level.num_models(); ++i)
        {
            const auto model = level.get_model(i);
            if (model.NumMeshes > 0xff00 || model.NumMeshes == 0)
            {
                continue;
            }

            auto meshes = load_meshes(model, version, mesh_storage);
            auto transforms = load_transforms(model, level);
            _models.push_back(model_source(model, meshes, transforms));
        }
    }

    std::weak_ptr<IModel> ModelStorage::find_by_type_id(uint16_t type_id) const
    {
        const uint16_t updated_type_id = get_skin_id(_platform_and_version, type_id);
        for (const auto& model : _models)
        {
            if (model->type_id() == updated_type_id)
            {
                return model;
            }
        }
        return {};
    }

    std::vector<std::weak_ptr<IModel>> ModelStorage::models() const
    {
        return _models | std::ranges::to<std::vector<std::weak_ptr<IModel>>>();
    }
}
