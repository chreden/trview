export module trview.app:ICompass;

import std;

import trview.common;

import :ICamera;

namespace trview
{
    export struct ICompass
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

    /// Get a string representation of a compass axis.
    /// @param axis The axis.
    /// @returns The name of the axis.
    export std::string axis_name(ICompass::Axis axis);

    /// Align a camera to a particular axis.
    /// @param camera The camera to adjust.
    /// @param axis The axis.
    export void align_camera_to_axis(ICamera& camera, ICompass::Axis axis);
}
