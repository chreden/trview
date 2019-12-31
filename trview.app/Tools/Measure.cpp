#include "Measure.h"
#include <trview.graphics/Device.h>
#include <sstream>
#include <iomanip>

#include <trview.app/Camera/ICamera.h>

using namespace trview::graphics;
using namespace DirectX::SimpleMath;

namespace trview
{
    Measure::Measure(const Device& device)
        : _mesh(create_cube_mesh(device))
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

    void Measure::render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        if (!_start.has_value() || !_end.has_value() || !_visible)
        {
            return;
        }

        using namespace DirectX::SimpleMath;

        auto to = _end.value() - _start.value();
        const auto scale = Matrix::CreateScale(0.05f);
        const auto view_projection = camera.view_projection();

        int blobs = static_cast<int>(to.Length() / 0.25f);

        to.Normalize();
        for (int i = 0; i <= blobs; ++i)
        {
            auto pos = _start.value() + to * 0.25f * static_cast<float>(i);
            auto wvp = scale * Matrix::CreateTranslation(pos) * view_projection;
            _mesh->render(context, wvp, texture_storage, Color(1.0f, 1.0f, 1.0f));
        }

        auto wvp = scale * Matrix::CreateTranslation(_end.value()) * view_projection;
        _mesh->render(context, wvp, texture_storage, Color(1.0f, 1.0f, 1.0f));

        auto halfway = Vector3::Lerp(_start.value(), _end.value(), 0.5f);
        const auto window_size = camera.view_size();

        Vector3 point = XMVector3Project(halfway, 0, 0, window_size.width, window_size.height, 0, 1.0f, camera.projection(), camera.view(), Matrix::Identity);

        on_position(Point(point.x, point.y));
    }

    std::wstring Measure::distance() const
    {
        std::wstringstream stream;
        stream << std::fixed << std::setprecision(2) << (_end.value() - _start.value()).Length();
        return stream.str();
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
