#pragma once

#include <trview.common/Event.h>
#include "PickInfo.h"
#include "PickResult.h"

namespace trview
{
    class Window;
    struct ICamera;

    class Picking final
    {
    public:
        /// Perform a pick operation.
        /// @param window The window that the scene is being rendered in.
        /// @param camera The current scene camera.
        void pick(const Window& window, const ICamera& camera);

        /// The sources of pick information.
        Event<PickInfo, PickResult&> pick_sources;

        /// Raise when something has been picked.
        Event<PickInfo, PickResult> on_pick;
    };
}
