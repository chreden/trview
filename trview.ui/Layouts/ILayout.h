#pragma once

namespace trview
{
    namespace ui
    {
        class Control;

        struct ILayout
        {
            virtual ~ILayout() = 0;
            /// <summary>
            /// Called when the layout has been added to a control. The layout should
            /// register for any events it needs to know about.
            /// </summary>
            /// <param name="control">The control to which the layout has been added.</param>
            virtual void bind(Control& control) = 0;
        };
    }
}
