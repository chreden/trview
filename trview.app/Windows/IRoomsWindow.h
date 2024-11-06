#pragma once

#include <cstdint>
#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include <trview.app/Elements/ITrigger.h>
#include "../Elements/CameraSink/ICameraSink.h"
#include "../Elements/ILight.h"
#include <trview.app/Elements/IRoom.h>
#include "../UI/MapColours.h"

namespace trview
{
    struct IRoomsWindow
    {
        using Source = std::function<std::shared_ptr<IRoomsWindow>()>;

        virtual ~IRoomsWindow() = 0;

        /// Event raised when the user has selected a room in the room window.
        Event<std::weak_ptr<IRoom>> on_room_selected;

        /// Event raised when the user has selected an item in the room window.
        Event<std::weak_ptr<IItem>> on_item_selected;

        /// Event raised when the user has selected a trigger in the room window.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        Event<> on_scene_changed;

        Event<std::weak_ptr<ISector>> on_sector_hover;

        Event<std::weak_ptr<ILight>> on_light_selected;
        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<IStaticMesh>> on_static_mesh_selected;

        /// Clear the selected trigger.
        virtual void clear_selected_trigger() = 0;

        /// Render the window.
        virtual void render() = 0;

        /// Set the current room that the viewer is focusing on.
        /// @param room The current room.
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Set the items in the level.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) = 0;
        /// <summary>
        /// Set the level version. This will control which information is displayed.
        /// </summary>
        /// <param name="version">The level version</param>
        virtual void set_level_version(trlevel::LevelVersion version) = 0;

        virtual void set_map_colours(const MapColours& colours) = 0;

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) = 0;

        /// Set the item currently selected in the viewer.
        /// @param item The item currently selected.
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) = 0;

        /// Set the trigger currently selected in the viewer.
        /// @param trigger The trigger currently selected.
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;

        /// <summary>
        /// Update the window.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;

        virtual void set_number(int32_t number) = 0;

        virtual void set_floordata(const std::vector<uint16_t>& data) = 0;

        virtual void set_selected_light(const std::weak_ptr<ILight>& light) = 0;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) = 0;

        virtual void clear_selected_light() = 0;
        virtual void clear_selected_camera_sink() = 0;
        virtual void set_ng_plus(bool value) = 0;
    };
}
