#pragma once

#include <optional>
#include <SimpleMath.h>
#include "Mesh.h"

namespace trview
{
    struct ICamera;

    namespace graphics
    {
        class Device;
    }

    namespace ui
    {
        class Control;
        class Label;
    }

    class Measure final
    {
    public:
        /// Create a new measure tool.
        /// @param device The device to use to create graphics resources.
        /// @param ui The control to add the measure label to.
        Measure(const graphics::Device& device, ui::Control& ui);

        /// Start measuring or reset the current measurement.
        void start();

        /// Add the position to the measurement.
        /// @param position The position to add to the measurement.
        /// @returns True if the measurement is complete.
        bool add(const DirectX::SimpleMath::Vector3& position);

        /// Set the position as the current temporary end of the measurement.
        /// @param position The position to use as the new end.
        void set(const DirectX::SimpleMath::Vector3& position);

        /// Render the measurement.
        /// @param context D3D device context.
        /// @param camera The camera being used to render the scene.
        /// @param texture_storage Texture storage for the level.
        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage);

        /// Get the current text version of the distance measured.
        /// @returns The text version of the distance.
        std::wstring distance() const;

        /// Get whether a distance is actively being measured.
        /// @returns True if start and end is set.
        bool measuring() const;
    private:
        std::optional<DirectX::SimpleMath::Vector3> _start;
        std::optional<DirectX::SimpleMath::Vector3> _end;
        ui::Label*                                  _label;
        std::unique_ptr<Mesh>                       _mesh;
    };
}
