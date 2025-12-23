#pragma once

#include "RoomsWindow.h"

namespace trview
{
    struct IRoomsWindowManager
    {
        virtual ~IRoomsWindowManager() = 0;

        Event<std::weak_ptr<ISector>> on_sector_hover;
        Event<std::weak_ptr<IStaticMesh>> on_static_mesh_selected;

        /// Render all of the rooms windows.
        virtual void render() = 0;
        /// Create a new rooms window.
        virtual std::weak_ptr<IRoomsWindow> create_window() = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;
        virtual std::vector<std::weak_ptr<IRoomsWindow>> windows() const = 0;
    };
}
