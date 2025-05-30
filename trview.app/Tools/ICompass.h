#pragma once

#include <trview.app/Camera/Camera.h>

#include <trview.common/Point.h>
#include <trview.common/Size.h>

namespace trview
{
    struct ICompass
    {
        /// The axis on the compass.
        enum class Axis
        {
            Pos_X,
            Pos_Y,
            Pos_Z,
            Neg_X,
            Neg_Y,
            Neg_Z
        };

        virtual ~ICompass() = 0;

        /// Render the compass.
        /// @param camera The current camera being used to view the level.
        virtual void render(const ICamera& camera) = 0;

        /// Pick against the compass points.
        /// @param mouse_position The mouse position in screen space.
        /// @param screen_size The screen size.
        /// @param axis The axis that was hovered over.
        virtual bool pick(const Point& mouse_position, const Size& screen_size, Axis& axis) = 0;

        /// Set whether the compass is visible.
        /// @param value Whether to render the compass.
        virtual void set_visible(bool value) = 0;
    };
}
