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

        /// Set the items in the current level.
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) = 0;
        /// Set the current room that the viewer is focusing on.
        /// @param room The current room.
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& items) = 0;

        /// Create a new rooms window.
        virtual std::weak_ptr<IRoomsWindow> create_window() = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;

        virtual void set_floordata(const std::vector<uint16_t>& data) = 0;
        virtual void set_trng(bool value) = 0;
        virtual std::vector<std::weak_ptr<IRoomsWindow>> windows() const = 0;
    };
}
