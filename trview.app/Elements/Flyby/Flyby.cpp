#include "Flyby.h"
#include <ranges>

namespace trview
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    IFlyby::~IFlyby()
    {
    }

    Flyby::Flyby(const std::shared_ptr<IMesh>& mesh, const std::weak_ptr<ILevel>& level)
        : _mesh(mesh), _level(level)
    {
    }

    void Flyby::initialise(const IFlybyNode::Source& flyby_node_source, const IMesh::Source& mesh_source, const std::vector<trlevel::tr4_flyby_camera>& camera_nodes)
    {
        _index = camera_nodes.empty() ? 0u : camera_nodes.front().sequence;
        _camera_nodes = camera_nodes | std::views::transform([&](auto&& n) { return flyby_node_source(n, weak_from_this(), _level); }) | std::ranges::to<std::vector>();
        _colour = Color(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX);
        generate_path(mesh_source);
    }

    void Flyby::render(const ICamera& camera, const DirectX::SimpleMath::Color&)
    {
        if (!_visible)
        {
            return;
        }

        for (const auto& node : _camera_nodes)
        {
            auto position = node->position();
            Vector3 target = node->direction();
            Vector3 direction;
            (target - position).Normalize(direction);
            const Vector3 angles = Quaternion::FromToRotation(Vector3::Backward, direction).ToEuler();
            const Matrix rotation = Matrix::CreateFromYawPitchRoll(angles.y, angles.x, DirectX::XMConvertToRadians(static_cast<float>(node->roll()) / 182.0f));
            const auto world = Matrix::CreateScale(0.2f) * rotation * Matrix::CreateTranslation(position);
            const auto wvp = world * camera.view_projection();
            auto light_direction = Vector3::TransformNormal(camera.position() - position, world.Invert());
            light_direction.Normalize();

            _mesh->render(wvp, _colour, 1.0f, light_direction);
        }

        if (_path_mesh)
        {
            const auto pos = _camera_nodes[0]->position();
            auto light_direction = Vector3::TransformNormal(camera.position() - pos, Matrix::Identity);
            light_direction.Normalize();
            _path_mesh->render(camera.view_projection(), _colour, 1.0f, light_direction);
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
        on_changed();
    }

    std::weak_ptr<ILevel> Flyby::level() const
    {
        return _level;
    }

    std::vector<std::weak_ptr<IFlybyNode>> Flyby::nodes() const
    {
        return _camera_nodes | std::ranges::to<std::vector<std::weak_ptr<IFlybyNode>>>();
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
            if (_camera_nodes[state.index]->flags() & 0x80)
            {
                state.index = _camera_nodes[state.index]->timer();
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
        state_at(state);
        return state;
    }

    void Flyby::generate_path(const IMesh::Source& mesh_source)
    {
        if (_camera_nodes.empty())
        {
            return;
        }

        // Samples per node - probably need more.
        const int path_samples = 10;
        const int num_vertices = 10;
        const float width = 0.01f;

        std::vector<std::vector<Vector3>> paths;
        paths.push_back({});

        for (uint32_t n = 0; n < _camera_nodes.size() - 1; ++n)
        {
            CameraState state;
            state.index = n;

            if (_camera_nodes[n]->flags() & 0x80)
            {
                n = _camera_nodes[n]->timer();
                state.index = n;
                paths.push_back({});
            }

            for (int s = 0; s <= path_samples; ++s)
            {
                state.t = static_cast<float>(s * (1.0f / path_samples));
                state_at(state);
                paths.back().push_back(state.position);
            }
        }

        std::vector<Vector3> vertices;
        std::vector<uint32_t> indices;

        for (uint32_t p = 0; p < paths.size(); ++p)
        {
            uint32_t path_base_index = static_cast<uint32_t>(vertices.size());

            const auto& points = paths[p];

            Vector3 direction;
            for (uint32_t n = 0; n < points.size(); ++n)
            {
                const Vector3 at = points[n];
                if (n == 0)
                {
                    vertices.push_back(at);
                    for (uint32_t v = path_base_index + 1; v <= num_vertices; ++v)
                    {
                        indices.push_back(path_base_index);
                        indices.push_back(v);
                        indices.push_back(v == num_vertices ? path_base_index + 1 : v + 1);
                    }
                }

                if (n == points.size() - 1)
                {
                    (at - points[n - 1]).Normalize(direction);
                }
                else
                {
                    (points[n + 1] - at).Normalize(direction);
                }

                for (int v = 0; v < num_vertices; ++v)
                {
                    const Matrix rotation =
                        Matrix::CreateRotationZ(v * (DirectX::g_XMTwoPi.f[0] / static_cast<float>(num_vertices))) *
                        Matrix::CreateFromQuaternion(Quaternion::FromToRotation(Vector3::Backward, direction));
                    const Vector3 pos = at + Vector3::TransformNormal(Vector3(0, width, 0), rotation);
                    vertices.push_back(pos);
                }

                const uint32_t base_index = path_base_index + 1 + n * num_vertices;
                base_index;

                if (n == points.size() - 1)
                {
                    vertices.push_back(at);
                    for (uint32_t v = 0; v < num_vertices; ++v)
                    {
                        indices.push_back(base_index + num_vertices);
                        indices.push_back(v == (num_vertices - 1) ? base_index : v + base_index + 1);
                        indices.push_back(v + base_index);
                    }
                }
                else
                {
                    for (uint32_t v = base_index; v < base_index + num_vertices; ++v)
                    {
                        indices.push_back(v);
                        indices.push_back(v + num_vertices);
                        indices.push_back(v == (base_index + num_vertices - 1) ? (base_index + num_vertices) : (v + num_vertices + 1));

                        indices.push_back(v);
                        indices.push_back(v == (base_index + num_vertices - 1) ? (base_index + num_vertices) : (v + num_vertices + 1));
                        indices.push_back(v == (base_index + num_vertices - 1) ? base_index : v + 1);
                    }
                }
            }
        }

        const Color colour = Colour::White;
        std::vector<Triangle> triangles;
        for (auto t = 0; t < indices.size(); t += 3)
        {
            triangles.push_back(
                {
                    .colours = { colour, colour, colour },
                    .texture_mode = Triangle::TextureMode::Untextured,
                    .vertices = { vertices[indices[t]], vertices[indices[t + 1]], vertices[indices[t + 2]] }
                });
        }
        
        _path_mesh = mesh_source(triangles);
    }

    void Flyby::state_at(CameraState& state) const
    {
        const int next_step = state.index + 1;
        const float t = state.t;

        const auto n0 = state.index == 0 ? _camera_nodes[state.index] : _camera_nodes[state.index - 1];
        const auto n1 = _camera_nodes[state.index];
        const auto n2 = _camera_nodes[next_step];
        const auto n3 = (next_step == _camera_nodes.size() - 1) ? _camera_nodes[next_step] : _camera_nodes[next_step + 1];
        state.position = Vector3::CatmullRom(n0->position(), n1->position(), n2->position(), n3->position(), t);
        Vector3 target = Vector3::CatmullRom(n0->direction(), n1->direction(), n2->direction(), n3->direction(), t);
        state.raw_direction = target;
        (target - state.position).Normalize(state.direction);

        state.roll = (n1->roll() + (n2->roll() - n1->roll()) * t) / -182.0f;
        state.fov = (n1->fov() + (n2->fov() - n1->fov()) * t) / 182.0f;
        state.speed = n1->speed() + (n2->speed() - n1->speed()) * t;
        state.timer = n1->timer();
        state.room_id = n1->room();
        state.flags = n1->flags();
    }

    PickResult Flyby::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        PickResult result{};

        for (const auto& node : _camera_nodes)
        {
            BoundingSphere sphere(node->position(), 0.125f);

            float distance = 0;
            if (sphere.Intersects(position, direction, distance))
            {
                result.distance = distance;
                result.hit = true;
                result.position = position + direction * distance;
                result.type = PickResult::Type::FlybyNode;
                result.flyby_node = std::const_pointer_cast<IFlybyNode>(node);
            }
        }

        return result;
    }

    int32_t Flyby::filterable_index() const
    {
        return static_cast<int32_t>(_index);
    }
}
