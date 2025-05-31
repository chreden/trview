#pragma once

#include <SimpleMath.h>
#include <trview.common/Event.h>
#include <trview.app/Camera/ICamera.h>

namespace trview
{
    struct IMeasure
    {
        virtual ~IMeasure() = 0;

        /// Event raised when the measure distance has changed.
        Event<float> on_distance;

        /// Event raised when the measure visibility has changed.
        Event<bool> on_visible;

        /// Event raised when the measure label position has moved.
        Event<Point> on_position;

        /// Start measuring or reset the current measurement.
        virtual void reset() = 0;

        /// Add the position to the measurement.
        /// @param position The position to add to the measurement.
        /// @returns True if the measurement is complete.
        virtual bool add(const DirectX::SimpleMath::Vector3& position) = 0;

        /// Set the position as the current temporary end of the measurement.
        /// @param position The position to use as the new end.
        virtual void set(const DirectX::SimpleMath::Vector3& position) = 0;

        /// Render the measurement.
        /// @param camera The camera being used to render the scene.
        virtual void render(const ICamera& camera) = 0;

        /// Get the current text version of the distance measured.
        /// @returns The text version of the distance.
        virtual std::string distance() const = 0;

        /// Get whether a distance is actively being measured.
        /// @returns True if start and end is set.
        virtual bool measuring() const = 0;

        /// Set whether the measure tool should be rendered.
        virtual void set_visible(bool value) = 0;
    };
}
