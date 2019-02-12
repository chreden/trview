#pragma once

#include <trview.common/Event.h>
#include "PickInfo.h"
#include "PickResult.h"

namespace trview
{
    namespace ui
    {
        class Control;
        class Label;
    }

    class Window;
    struct ICamera;

    class Picking final
    {
    public:
        explicit Picking(ui::Control& parent);

        /// Perform a pick operation.
        /// @param window The window that the scene is being rendered in.
        /// @param camera The current scene camera.
        void pick(const Window& window, const ICamera& camera);

        /// Set whether to show the picking tooltip.
        /// @param value Whether to show the tooltip.
        void set_show(bool value);

        /// The sources of pick information.
        Event<PickInfo, PickResult&> pick_sources;

        /// Raise when something has been picked.
        Event<PickResult> on_pick;
    private:
        ui::Label* _label;
        bool       _show{ true };
        PickResult _current_pick;
    };
}
