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
        Picking(ui::Control& parent);

        /// Perform a pick operation.
        /// @param window The window that the scene is being rendered in.
        /// @param camera The current scene camera.
        void pick(const Window& window, const ICamera& camera);

        Event<PickInfo, PickResult&> on_pick;
    private:
        ui::Label* _label;
        bool       _show{ true };
        PickResult _current_pick;
    };
}
