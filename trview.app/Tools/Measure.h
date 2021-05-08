#pragma once

#include <optional>
#include <trview.app/Geometry/IMesh.h>
#include "IMeasure.h"

namespace trview
{
    class Measure final : public IMeasure
    {
    public:
        /// Create a new measure tool.
        /// @param device The device to use to create graphics resources.
        explicit Measure(const std::shared_ptr<graphics::IDevice>& device,
            const IMesh::Source& mesh_source);

        virtual ~Measure() = default;

        /// Start measuring or reset the current measurement.
        virtual void reset() override;

        /// Add the position to the measurement.
        /// @param position The position to add to the measurement.
        /// @returns True if the measurement is complete.
        virtual bool add(const DirectX::SimpleMath::Vector3& position) override;

        /// Set the position as the current temporary end of the measurement.
        /// @param position The position to use as the new end.
        virtual void set(const DirectX::SimpleMath::Vector3& position) override;

        /// Render the measurement.
        /// @param camera The camera being used to render the scene.
        /// @param texture_storage Texture storage for the level.
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) override;

        /// Get the current text version of the distance measured.
        /// @returns The text version of the distance.
        virtual std::wstring distance() const override;

        /// Get whether a distance is actively being measured.
        /// @returns True if start and end is set.
        virtual bool measuring() const override;

        /// Set whether the measure tool should be rendered.
        virtual void set_visible(bool value) override;
    private:
        std::shared_ptr<graphics::IDevice> _device;
        std::optional<DirectX::SimpleMath::Vector3> _start;
        std::optional<DirectX::SimpleMath::Vector3> _end;
        std::shared_ptr<IMesh> _mesh;
        bool _visible{ true };
    };
}
