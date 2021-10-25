#include "Mover.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    Mover::Mover(const IMesh::Source& mesh_source)
        : _mesh(create_cube_mesh(mesh_source))
    {
    }

    void Mover::render(const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        Matrix world = Matrix::Identity;
        _mesh->render(world * camera.view_projection(), texture_storage, Colour(1, 0, 1), 1.0f, camera.forward());
    }
}