#pragma once

#include <SimpleMath.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.app/Elements/IRoom.h>
#include <trview.app/Elements/IEntity.h>
#include <trview.app/Elements/ILight.h>
#include <trview.common/Event.h>

namespace trview
{
    struct ILevel
    {
        using Source = std::function<std::unique_ptr<ILevel>(std::unique_ptr<trlevel::ILevel>)>;

        enum class RoomHighlightMode
        {
            None,
            Highlight,
            Neighbours
        };

        virtual ~ILevel() = 0;
        /// Gets whether the specified alternate group is active.
        /// @param group The group to check.
        /// @returns True if the group is active.
        virtual bool alternate_group(uint32_t group) const = 0;
        /// Gets the alternate groups that exist in the level.
        /// @returns The set of alternate groups in the level.
        virtual std::set<uint32_t> alternate_groups() const = 0;
        // Get the current state of the alternate mode (flipmap).
        virtual bool alternate_mode() const = 0;
        /// Determines if there are any flipmaps in the level.
        /// @returns True if there are flipmaps.
        virtual bool any_alternates() const = 0;
        virtual std::string filename() const = 0;
        virtual bool highlight_mode_enabled(RoomHighlightMode mode) const = 0;
        /// Get the items in this level.
        /// @returns All items in the level.
        virtual std::vector<Item> items() const = 0;
        virtual std::vector<graphics::Texture> level_textures() const = 0;
        virtual std::vector<std::weak_ptr<ILight>> lights() const = 0;
        /// Get the number of rooms in the level.
        virtual uint32_t number_of_rooms() const = 0;
        virtual void on_camera_moved() = 0;
        // Determine whether the specified ray hits any of the triangles in any of the room geometry.
        // position: The world space position of the source of the ray.
        // direction: The direction of the ray.
        // Returns: The result of the operation. If 'hit' is true, distance and position contain
        // how far along the ray the hit was and the position in world space. The room that was hit
        // is also specified.
        virtual PickResult pick(const ICamera& camera, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        /// Render the current scene.
        /// @param camera The current camera.
        /// @param render_selection Whether to render selection highlights on selected items.
        virtual void render(const ICamera& camera, bool render_selection) = 0;
        /// Render the transparent triangles in the scene.
        /// @param camera The current camera.
        virtual void render_transparency(const ICamera& camera) = 0;
        // Returns the room with ID provided 
        virtual std::weak_ptr<IRoom> room(uint32_t id) const = 0;
        virtual std::vector<RoomInfo> room_info() const = 0;
        virtual RoomInfo room_info(uint32_t room) const = 0;
        virtual std::vector<std::weak_ptr<IRoom>> rooms() const = 0;
        virtual uint16_t selected_room() const = 0;
        // Set whether to render the alternate mode (the flipmap) or the regular room.
        // enabled: Whether to render the flipmap.
        virtual void set_alternate_mode(bool enabled) = 0;
        /// Set whether to render the alternate group specified.
        /// @param group The group to toggle.
        /// @param enabled Whether the group is enabled.
        virtual void set_alternate_group(uint32_t group, bool enabled) = 0;
        virtual void set_filename(const std::string& filename) = 0;
        virtual void set_highlight_mode(RoomHighlightMode mode, bool enabled) = 0;
        virtual void set_item_visibility(uint32_t index, bool state) = 0;
        virtual void set_selected_trigger(uint32_t number) = 0;
        virtual void set_selected_light(uint32_t number) = 0;
        virtual void set_show_hidden_geometry(bool show) = 0;
        virtual void set_show_triggers(bool show) = 0;
        virtual void set_show_water(bool show) = 0;
        virtual void set_show_wireframe(bool show) = 0;
        virtual void set_show_bounding_boxes(bool show) = 0;
        virtual void set_show_lights(bool show) = 0;
        virtual void set_trigger_visibility(uint32_t index, bool state) = 0;
        virtual void set_use_trle_colours(bool value) = 0;
        virtual void set_neighbour_depth(uint32_t depth) = 0;
        virtual void set_selected_room(uint16_t index) = 0;
        virtual void set_selected_item(uint32_t index) = 0;
        virtual void set_light_visibility(uint32_t index, bool state) = 0;
        virtual bool show_hidden_geometry() const = 0;
        virtual bool show_lights() const = 0;
        virtual bool show_triggers() const = 0;
        virtual const ILevelTextureStorage& texture_storage() const = 0;
        /// Get the triggers in this level.
        /// @returns All triggers in the level.
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const = 0;
        virtual bool use_trle_colours() const = 0;
        virtual trlevel::LevelVersion version() const = 0;
        // Event raised when the level needs to change the selected room.
        Event<uint16_t> on_room_selected;
        // Event raised when the level needs to change the alternate mode.
        Event<bool> on_alternate_mode_selected;
        /// Event raised when the level needs to change the alternate group mode.
        Event<uint16_t, bool> on_alternate_group_selected;
        /// Event raised when something has changed in the appearance of the level or the
        /// items that are contained within.
        Event<> on_level_changed;
    };
}
