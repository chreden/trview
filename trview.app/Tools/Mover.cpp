#include "Mover.h"
#include <trview.common/Maths.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    Mover::Mover(const IMesh::Source& mesh_source)
        : _mesh(create_cube_mesh(mesh_source))
    {
    }

    void Mover::render(const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        Matrix world = Matrix::CreateTranslation(_position);
        Matrix view_projection = camera.view_projection();
        Matrix wvp = world * view_projection;
        const auto nodule_scale = Matrix::CreateScale(0.05f);

        const float thickness = 0.015f;
        const auto scale = Matrix::CreateScale(thickness, 1.0f, thickness);

        // Render rods
        _mesh->render(scale * wvp, texture_storage, Color(1.0f, 0.0f, 0.0f));
        _mesh->render(scale * Matrix::CreateRotationZ(maths::HalfPi) * wvp, texture_storage, Color(0.0f, 1.0f, 0.0f));
        _mesh->render(scale * Matrix::CreateRotationX(maths::HalfPi) * wvp, texture_storage, Color(0.0f, 0.0f, 1.0f));

        // Render nodules
        // Y
        _mesh->render(nodule_scale * Matrix::CreateTranslation(0, 0.5f, 0) * wvp, texture_storage, Color(1.0f, 0.0f, 0.0f));
        _mesh->render(nodule_scale * Matrix::CreateTranslation(0, -0.5f, 0) * wvp, texture_storage, Color(1.0f, 0.0f, 0.0f));
        // X
        _mesh->render(nodule_scale * Matrix::CreateTranslation(0.5f, 0, 0) * wvp, texture_storage, Color(0.0f, 1.0f, 0.0f));
        _mesh->render(nodule_scale * Matrix::CreateTranslation(-0.5f, 0, 0) * wvp, texture_storage, Color(0.0f, 1.0f, 0.0f));
        // Z
        _mesh->render(nodule_scale * Matrix::CreateTranslation(0, 0, 0.5f) * wvp, texture_storage, Color(0.0f, 0.0f, 1.0f));
        _mesh->render(nodule_scale * Matrix::CreateTranslation(0, 0, -0.5f) * wvp, texture_storage, Color(0.0f, 0.0f, 1.0f));
    }

    void Mover::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        _position = position;
    }
}