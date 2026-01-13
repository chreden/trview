#pragma once

#include <SimpleMath.h>
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "../Elements/IRoom.h"
#include "../Elements/IStaticMesh.h"
#include "../Lua/Scriptable/IScriptable.h"
#include <trview.app/Elements/ILight.h>
#include "CameraSink/ICameraSink.h"
#include "Flyby/IFlyby.h"
#include <trview.common/Event.h>
#include "../UI/MapColours.h"
#include <trlevel/IPack.h>
#include <trview.common/Messages/IRecipient.h>

namespace trview
{
    struct ISoundStorage;
    struct ISoundSource;

    struct ILevel : public IRecipient
    {
        using Source = std::function<std::shared_ptr<ILevel>(const std::string&, const std::shared_ptr<trlevel::IPack>&, trlevel::ILevel::LoadCallbacks)>;

        enum class OpenMode
        {
            Full,
            Reload
        };

        enum class RoomHighlightMode
        {
            None,
            Highlight,
            Neighbours
        };

        virtual ~ILevel() = 0;
        virtual void add_scriptable(const std::weak_ptr<IScriptable>& scriptable) = 0;
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
        virtual std::weak_ptr<ICameraSink> camera_sink(uint32_t index) const = 0;
        virtual std::vector<std::weak_ptr<ICameraSink>> camera_sinks() const = 0;
        virtual std::string filename() const = 0;
        virtual bool has_model(uint32_t type_id) const = 0;
        virtual std::vector<uint16_t> floor_data() const = 0;
        virtual bool highlight_mode_enabled(RoomHighlightMode mode) const = 0;
        virtual bool is_in_visible_set(const std::weak_ptr<IRoom>& room) const = 0;
        virtual std::weak_ptr<IItem> item(uint32_t index) const = 0;
        /// Get the items in this level.
        /// @returns All items in the level.
        virtual std::vector<std::weak_ptr<IItem>> items() const = 0;
        virtual std::vector<graphics::Texture> level_textures() const = 0;
        virtual std::weak_ptr<ILight> light(uint32_t index) const = 0;
        virtual std::vector<std::weak_ptr<ILight>> lights() const = 0;
        virtual MapColours map_colours() const = 0;
        virtual std::string name() const = 0;
        virtual uint32_t neighbour_depth() const = 0;
        virtual bool ng_plus() const = 0;
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
        virtual trlevel::Platform platform() const = 0;
        virtual trlevel::PlatformAndVersion platform_and_version() const = 0;
        /// Render the current scene.
        /// @param camera The current camera.
        /// @param render_selection Whether to render selection highlights on selected items.
        virtual void render(const ICamera& camera, bool render_selection) = 0;
        /// Render the transparent triangles in the scene.
        /// @param camera The current camera.
        virtual void render_transparency(const ICamera& camera) = 0;
        // Returns the room with ID provided 
        virtual std::weak_ptr<IRoom> room(uint32_t id) const = 0;
        virtual std::vector<std::weak_ptr<IRoom>> rooms() const = 0;
        virtual std::vector<std::weak_ptr<IScriptable>> scriptables() const = 0;
        virtual std::optional<uint32_t> selected_item() const = 0;
        virtual std::optional<uint32_t> selected_light() const = 0;
        virtual std::optional<uint32_t> selected_camera_sink() const = 0;
        virtual std::weak_ptr<IRoom> selected_room() const = 0;
        virtual std::optional<uint32_t> selected_trigger() const = 0;
        // Set whether to render the alternate mode (the flipmap) or the regular room.
        // enabled: Whether to render the flipmap.
        virtual void set_alternate_mode(bool enabled) = 0;
        /// Set whether to render the alternate group specified.
        /// @param group The group to toggle.
        /// @param enabled Whether the group is enabled.
        virtual void set_alternate_group(uint32_t group, bool enabled) = 0;
        virtual void set_filename(const std::string& filename) = 0;
        virtual void set_highlight_mode(RoomHighlightMode mode, bool enabled) = 0;
        virtual void set_map_colours(const MapColours& map_colours) = 0;
        virtual void set_selected_trigger(uint32_t number) = 0;
        virtual void set_selected_light(uint32_t number) = 0;
        virtual void set_selected_flyby_node(const std::weak_ptr<IFlybyNode>& node) = 0;
        virtual void set_show_geometry(bool show) = 0;
        virtual void set_show_triggers(bool show) = 0;
        virtual void set_show_water(bool show) = 0;
        virtual void set_show_wireframe(bool show) = 0;
        virtual void set_show_bounding_boxes(bool show) = 0;
        virtual void set_show_lighting(bool show) = 0;
        virtual void set_show_lights(bool show) = 0;
        virtual void set_show_items(bool show) = 0;
        virtual void set_show_rooms(bool show) = 0;
        virtual void set_show_camera_sinks(bool show) = 0;
        virtual void set_show_sound_sources(bool show) = 0;
        virtual void set_show_animation(bool show) = 0;
        virtual void set_neighbour_depth(uint32_t depth) = 0;
        virtual void set_selected_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) = 0;
        virtual void set_ng_plus(bool show) = 0;
        virtual bool show_camera_sinks() const = 0;
        virtual bool show_geometry() const = 0;
        virtual bool show_lighting() const = 0;
        virtual bool show_lights() const = 0;
        virtual bool show_triggers() const = 0;
        virtual bool show_items() const = 0;
        virtual bool show_sound_sources() const = 0;
        virtual std::vector<std::weak_ptr<ISoundSource>> sound_sources() const = 0;
        virtual std::vector<std::weak_ptr<IStaticMesh>> static_meshes() const = 0;
        virtual std::shared_ptr<ILevelTextureStorage> texture_storage() const = 0;
        virtual std::weak_ptr<IStaticMesh> static_mesh(uint32_t index) const = 0;
        /// <summary>
        /// Get the trigger at the specific index.
        /// </summary>
        /// <param name="index">Trigger index.</param>
        /// <returns>The trigger or an empty pointer.</returns>
        virtual std::weak_ptr<ITrigger> trigger(uint32_t index) const = 0;
        /// <summary>
        /// Get the triggers in this level.
        /// </summary>
        /// <returns>All triggers in the level.</returns>
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const = 0;
        virtual trlevel::LevelVersion version() const = 0;
        virtual std::weak_ptr<ISoundStorage> sound_storage() const = 0;
        virtual bool trng() const = 0;
        virtual std::weak_ptr<trlevel::IPack> pack() const = 0;
        virtual std::string hash() const = 0;
        virtual std::vector<std::weak_ptr<IFlyby>> flybys() const = 0;
        virtual void update(float delta) = 0;

        // Event raised when the level needs to change the alternate mode.
        Event<bool> on_alternate_mode_selected;
        /// Event raised when the level needs to change the alternate group mode.
        Event<uint16_t, bool> on_alternate_group_selected;
        mutable Event<> on_geometry_colours_changed;
    };

    /// <summary>
    /// Find the last item with the type id specified.
    /// </summary>
    /// <param name="level">The level to search.</param>
    /// <param name="type_id">The type id to search for.</param>
    /// <param name="output_item">The item to output the result into.</param>
    /// <returns>True if the item was found.</returns>
    bool find_last_item_by_type_id(const ILevel& level, uint32_t type_id, std::weak_ptr<IItem>& output_item);
}
