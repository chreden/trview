#pragma once

#include <cstdint>
#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include <trview.app/Elements/ITrigger.h>
#include "../Elements/CameraSink/ICameraSink.h"
#include "../Elements/ILight.h"
#include <trview.app/Elements/IRoom.h>
#include "../UI/MapColours.h"
#include "../Filters/Filters.h"

namespace trview
{
    struct IRoomsWindow
    {
        using Source = std::function<std::shared_ptr<IRoomsWindow>()>;

        virtual ~IRoomsWindow() = 0;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        Event<std::weak_ptr<ISector>> on_sector_hover;

        Event<std::weak_ptr<IStaticMesh>> on_static_mesh_selected;

        /// Clear the selected trigger.
        virtual void clear_selected_trigger() = 0;

        /// Render the window.
        virtual void render() = 0;

        /// <summary>
        /// Update the window.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;

        virtual void set_number(int32_t number) = 0;

        virtual void clear_selected_light() = 0;
        virtual void clear_selected_camera_sink() = 0;
        virtual std::string name() const = 0;
        virtual void set_filters(std::vector<Filters<IRoom>::Filter> filters) = 0;
    };
}
