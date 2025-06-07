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

    Flyby::Flyby(const std::shared_ptr<IMesh>& mesh, const std::vector<trlevel::tr4_flyby_camera>& camera_nodes, uint32_t index)
        : _mesh(mesh), _camera_nodes(camera_nodes), _index(index)
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
            auto position = to_position(node);
            Vector3 target = to_direction(node);
            Vector3 direction;
            (target - position).Normalize(direction);
            const Vector3 angles = Quaternion::FromToRotation(Vector3::Backward, direction).ToEuler();
            const Matrix rotation = Matrix::CreateFromYawPitchRoll(angles.y, angles.x, DirectX::XMConvertToRadians(static_cast<float>(node.roll) / 182.0f));
            const auto world = Matrix::CreateScale(0.2f) * rotation * Matrix::CreateTranslation(position);
            const auto wvp = world * camera.view_projection();
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

    std::vector<trlevel::tr4_flyby_camera> Flyby::nodes() const
    {
        return _camera_nodes;
    }

    uint32_t Flyby::number() const
    {
        return _index;
    }

    [[nodiscard]] IFlyby::CameraState Flyby::update_state(const CameraState& base_state, float delta) const
    {
        CameraState state = base_state;

        // 1. Check for invalid state (ended, etc).
        if (state.state == CameraState::State::Ended)
        {
            return state;
        }

        // 2. Advance time
        state.t += delta * (30 * (static_cast<float>(state.speed) / 65535.0f));

        // 3. Check for state changed (new node etc)
        if (state.t >= 1.0f)
        {
            ++state.index;
            if (_camera_nodes[state.index].flags & 0x80)
            {
                state.index = _camera_nodes[state.index].timer;
            }
            state.t = 0;
        }

        // 4. Check for the end
        if (state.index >= _camera_nodes.size() - 1) 
        {
            state.state = CameraState::State::Ended;
            return state;
        }

        // 5. Calculate outputs
        const int next_step = state.index + 1;
        const float t = state.t;

        const auto n0 = state.index == 0 ? _camera_nodes[state.index] : _camera_nodes[state.index - 1];
        const auto n1 = _camera_nodes[state.index];
        const auto n2 = _camera_nodes[next_step];
        const auto n3 = (next_step == _camera_nodes.size() - 1) ? _camera_nodes[next_step] : _camera_nodes[next_step + 1];
        state.position = Vector3::CatmullRom(to_position(n0), to_position(n1), to_position(n2), to_position(n3), t);
        Vector3 target = Vector3::CatmullRom(to_direction(n0), to_direction(n1), to_direction(n2), to_direction(n3), t);
        state.raw_direction = target;
        (target - state.position).Normalize(state.direction);

        state.roll = (n1.roll + (n2.roll - n1.roll) * t) / -182.0f;
        state.fov = (n1.fov + (n2.fov - n1.fov) * t) / 182.0f;
        state.speed = n1.speed + (n2.speed - n1.speed) * t;
        state.timer = n1.timer;
        state.room_id = n1.room_id;
        state.flags = n1.flags;
        return state;
    }
}
