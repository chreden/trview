#include "Flyby.h"

namespace trview
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    namespace
    {
        Vector3 to_vector(int32_t x, int32_t y, int32_t z)
        {
            return Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) / trlevel::Scale;
        }
    }

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

    IFlyby::CameraState Flyby::state_at(float value) const
    {
        CameraState state;

        if (_camera_nodes.empty())
        {
            return state;
        }

        if (_camera_nodes.size() == 1)
        {
            state.position = to_vector(_camera_nodes[0].x, _camera_nodes[0].y, _camera_nodes[0].z);
            return state;
        }

        // With: 
        // nodes: 2
        // step = 1.0f / 1 = 1.0f
        // value: 0.5
        // at_step = 0.5f / 1.0f = 0.5 -> 0
        // between_nodes = value - (at_step * step)
        //       = 0.5
        // t = between_nodes * step
        //       0.5

        const float step = 1.0f / static_cast<float>(_camera_nodes.size() - 1);
        const int at_step = static_cast<int>(value / step);
        const int next_step = at_step + 1;
        const float between_nodes = value - (static_cast<float>(at_step) * step);
        const float t = between_nodes / step;

        if (next_step >= _camera_nodes.size())
        {
            const auto node = _camera_nodes.back();
            state.position = to_vector(node.x, node.y, node.z);
            return state;
        }

        const auto node = _camera_nodes[at_step];
        const auto next_node = _camera_nodes[next_step];
        state.position = DirectX::XMVectorLerp(to_vector(node.x, node.y, node.z), to_vector(next_node.x, next_node.y, next_node.z), t);
        const Vector3 target = DirectX::XMVectorLerp(to_vector(node.dx, node.dy, node.dz), to_vector(next_node.dx, next_node.dy, next_node.dz), t);
        (target - state.position).Normalize(state.direction);

        state.roll = (node.roll + (next_node.roll - node.roll) * t) / -182.0f;
        return state;
    }
}
