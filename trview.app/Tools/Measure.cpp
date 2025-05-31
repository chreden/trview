#include "Measure.h"
#include <trview.graphics/IDevice.h>
#include <trview.app/Camera/ICamera.h>

using namespace trview::graphics;
using namespace DirectX::SimpleMath;

namespace trview
{
    IMeasure::~IMeasure()
    {
    }

    Measure::Measure(const std::shared_ptr<IDevice>& device, const IMesh::Source& mesh_source)
        : _device(device), _mesh(create_cube_mesh(mesh_source))
    {
    }

    void Measure::reset()
    {
        _start.reset();
        _end.reset();
        on_visible(false);
    }

    bool Measure::add(const Vector3& position)
    {
        if (!_start.has_value())
        {
            _start = position;
        }
        else
        {
            _end = position;
            on_visible(true);
            return true;
        }

        return false;
    }

    void Measure::set(const Vector3& position)
    {
        if (!_start.has_value())
        {
            return;
        }

        _end = position;
        on_distance((_end.value() - _start.value()).Length());
    }

    void Measure::render(const ICamera& camera)
    {
        if (!_start.has_value() || !_end.has_value() || !_visible)
        {
            return;
        }

        using namespace DirectX::SimpleMath;

        auto context = _device->context();
        auto to = _end.value() - _start.value();
        auto halfway = Vector3::Lerp(_start.value(), _end.value(), 0.5f);

        auto from_camera = halfway - camera.position();
        from_camera.Normalize();
        if (camera.forward().Dot(from_camera) < 0)
        {
            on_visible(false);
            return;
        }

        on_visible(true);

        const auto scale = Matrix::CreateScale(0.05f);
        const auto view_projection = camera.view_projection();

        int blobs = static_cast<int>(to.Length() / 0.25f);

        to.Normalize();
        for (int i = 0; i <= blobs; ++i)
        {
            auto pos = _start.value() + to * 0.25f * static_cast<float>(i);
            auto wvp = scale * Matrix::CreateTranslation(pos) * view_projection;
            _mesh->render(wvp, Color(1.0f, 1.0f, 1.0f));
        }

        auto wvp = scale * Matrix::CreateTranslation(_end.value()) * view_projection;
        _mesh->render(wvp, Color(1.0f, 1.0f, 1.0f));

        const auto window_size = camera.view_size();

        Vector3 point = XMVector3Project(halfway, 0, 0, window_size.width, window_size.height, 0, 1.0f, camera.projection(), camera.view(), Matrix::Identity);

        on_position(Point(point.x, point.y));
    }

    std::string Measure::distance() const
    {
        return std::format("{:.2f}", (_end.value() - _start.value()).Length());
    }

    bool Measure::measuring() const
    {
        return _start.has_value() && _end.has_value();
    }

    void Measure::set_visible(bool value)
    {
        _visible = value;
        on_visible(value && _end.has_value());
    }
}
