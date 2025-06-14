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

        Vector3 to_position(const trlevel::tr4_flyby_camera& node)
        {
            return to_vector(node.x, node.y, node.z);
        }

        Vector3 to_direction(const trlevel::tr4_flyby_camera& node)
        {
            return to_vector(node.dx, node.dy, node.dz);
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
            auto world = Matrix::CreateScale(0.1f) * Matrix::CreateTranslation(position);
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
            state.position = to_position(_camera_nodes[0]);
            state.direction = to_direction(_camera_nodes[0]);
            state.fov = _camera_nodes[0].fov;
            state.roll = _camera_nodes[0].roll;
            state.index = 0;
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
        state.index = at_step;
        state.t = t;

        if (next_step >= _camera_nodes.size())
        {
            const auto node = _camera_nodes.back();
            state.position = to_vector(node.x, node.y, node.z);
            return state;
        }

        const auto n0 = at_step == 0 ? _camera_nodes[at_step] : _camera_nodes[at_step - 1];
        const auto n1 = _camera_nodes[at_step];
        const auto n2 = _camera_nodes[next_step];
        const auto n3 = (next_step == _camera_nodes.size() - 1) ? _camera_nodes[next_step] : _camera_nodes[next_step + 1];
        state.position = XMVectorCatmullRom(to_position(n0), to_position(n1), to_position(n2), to_position(n3), t);
        Vector3 target = XMVectorCatmullRom(to_direction(n0), to_direction(n1), to_direction(n2), to_direction(n3), t);
        (target - state.position).Normalize(state.direction);

        state.roll = (n1.roll + (n2.roll - n1.roll) * t) / -182.0f;
        state.fov = (n1.fov + (n2.fov - n1.fov) * t) / 182.0f;
        return state;
    }

    [[nodiscard]] IFlyby::CameraState Flyby::update_state(const CameraState& base_state, float delta) const
    {
        CameraState state = base_state;

        if (_camera_nodes.empty())
        {
            return state;
        }

        if (_camera_nodes.size() == 1)
        {
            state.position = to_position(_camera_nodes[0]);
            state.direction = to_direction(_camera_nodes[0]);
            state.fov = _camera_nodes[0].fov;
            state.roll = _camera_nodes[0].roll;
            state.index = 0;
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

        state.value += delta * 0.05f;
        float value = state.value;

        const float step = 1.0f / static_cast<float>(_camera_nodes.size() - 1);
        int at_step = static_cast<int>(value / step);

        // Bit 7 handling - jump to specified index.
        if (at_step != base_state.index && (_camera_nodes[at_step].flags & 0x80))
        {
            at_step = _camera_nodes[at_step].timer;
            value = at_step * step;
            state.value = value;
        }

        if (at_step >= _camera_nodes.size() - 1)
        {
            state.index = static_cast<int32_t>(_camera_nodes.size() - 1);
            state.value = 1.0f;
            state.position = to_position(_camera_nodes.back());
            state.direction = to_direction(_camera_nodes.back());
            state.roll = _camera_nodes.back().roll;
            state.fov = _camera_nodes.back().fov;
            return state;
        }

        const int next_step = at_step + 1;
        const float between_nodes = value - (static_cast<float>(at_step) * step);
        const float t = between_nodes / step;
        state.index = at_step;
        state.t = t;

        if (next_step >= _camera_nodes.size())
        {
            const auto node = _camera_nodes.back();
            state.position = to_vector(node.x, node.y, node.z);
            return state;
        }

        const auto n0 = at_step == 0 ? _camera_nodes[at_step] : _camera_nodes[at_step - 1];
        const auto n1 = _camera_nodes[at_step];
        const auto n2 = _camera_nodes[next_step];
        const auto n3 = (next_step == _camera_nodes.size() - 1) ? _camera_nodes[next_step] : _camera_nodes[next_step + 1];
        state.position = XMVectorCatmullRom(to_position(n0), to_position(n1), to_position(n2), to_position(n3), t);
        Vector3 target = XMVectorCatmullRom(to_direction(n0), to_direction(n1), to_direction(n2), to_direction(n3), t);
        (target - state.position).Normalize(state.direction);

        state.roll = (n1.roll + (n2.roll - n1.roll) * t) / -182.0f;
        state.fov = (n1.fov + (n2.fov - n1.fov) * t) / 182.0f;
        return state;
    }
}
