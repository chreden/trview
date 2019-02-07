#include "Measure.h"
#include <trview.graphics/Device.h>
#include <trview.ui/Label.h>
#include <sstream>
#include <iomanip>

#include "ICamera.h"

using namespace trview::graphics;
using namespace DirectX::SimpleMath;

namespace trview
{
    Measure::Measure(const Device& device, ui::Control& ui)
        : _mesh(create_cube_mesh(device.device()))
    {
        auto label = std::make_unique<ui::Label>(Point(300, 100), Size(50, 30), Colour(1.0f, 0.2f, 0.2f, 0.2f), L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        label->set_visible(false);
        _label = ui.add_child(std::move(label));
    }

    void Measure::reset()
    {
        _start.reset();
        _end.reset();
        _label->set_visible(false);
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
            _label->set_visible(true);
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
        std::wstringstream stream;
        stream << std::fixed << std::setprecision(2) << (_end.value() - _start.value()).Length();
        _label->set_text(stream.str());
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

        int blobs = to.Length() / 0.25f;

        to.Normalize();
        for (int i = 0; i <= blobs; ++i)
        {
            auto pos = _start.value() + to * 0.25f * i;
            auto wvp = scale * Matrix::CreateTranslation(pos) * view_projection;
            _mesh->render(context, wvp, texture_storage, Color(1.0f, 1.0f, 1.0f));
        }

        auto wvp = scale * Matrix::CreateTranslation(_end.value()) * view_projection;
        _mesh->render(context, wvp, texture_storage, Color(1.0f, 1.0f, 1.0f));

        auto halfway = Vector3::Lerp(_start.value(), _end.value(), 0.5f);
        const auto window_size = camera.view_size();

        Vector3 point = XMVector3Project(halfway, 0, 0, window_size.width, window_size.height, 0, 1.0f, camera.projection(), camera.view(), Matrix::Identity);

        _label->set_position(Point(point.x, point.y));
    }

    std::wstring Measure::distance() const
    {
        return _label->text();
    }

    bool Measure::measuring() const
    {
        return _start.has_value() && _end.has_value();
    }

    void Measure::set_visible(bool value)
    {
        _visible = value;
        _label->set_visible(value && _end.has_value());
    }
}
