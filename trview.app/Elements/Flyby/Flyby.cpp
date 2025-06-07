#include "Flyby.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    IFlyby::~IFlyby()
    {
    }

    Flyby::Flyby(const std::shared_ptr<IMesh>& mesh, const std::vector<trlevel::tr4_flyby_camera>& camera_nodes)
        : _mesh(mesh), _camera_nodes(camera_nodes)
    {
        _colour = Color(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX);
    }

    void Flyby::render(const ICamera& camera, const DirectX::SimpleMath::Color&)
    {
        if (!_visible)
        {
            return;
        }

        for (const auto& node : _camera_nodes)
        {
            const auto position = Vector3(static_cast<float>(node.x), static_cast<float>(node.y), static_cast<float>(node.z)) / trlevel::Scale;
            auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(position);
            auto wvp = world * camera.view_projection();
            auto light_direction = Vector3::TransformNormal(camera.position() - position, world.Invert());
            light_direction.Normalize();
            _mesh->render(wvp, _colour, 1.0f, light_direction);
        }
    }

    void Flyby::get_transparent_triangles(ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&)
    {
    }

    bool Flyby::visible() const
    {
        return _visible;
    }

    void Flyby::set_visible(bool value)
    {
        _visible = value;
    }
}
