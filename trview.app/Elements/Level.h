#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <SimpleMath.h>
#include <set>

#include <trview.graphics/Texture.h>
#include <trview.common/Event.h>
#include <trlevel/ILevel.h>

#include "Room.h"
#include "Entity.h"
#include <trview.app/Geometry/Mesh.h>
#include "StaticMesh.h"
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>

#include <trview.app/Graphics/IMeshStorage.h>

#include <trview.graphics/RenderTarget.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct ICamera;
    class SelectionRenderer;
    struct ITypeNameLookup;

    namespace graphics
    {
        struct IShaderStorage;
        struct IShader;
    }

    class Level
    {
    public:
        Level(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, std::unique_ptr<trlevel::ILevel>&& level, const ITypeNameLookup& type_names);
        ~Level();

        enum class RoomHighlightMode
        {
            None,
            Highlight,
            Neighbours
        };

        // Temporary, for the room info and texture window.
        std::vector<RoomInfo> room_info() const;
        RoomInfo room_info(uint32_t room) const;
        std::vector<graphics::Texture> level_textures() const;

        uint16_t selected_room() const;

        /// Get the items in this level.
        /// @returns All items in the level.
        const std::vector<Item>& items() const;

        /// Get the number of rooms in the level.
        uint32_t number_of_rooms() const;

        /// Get the triggers in this level.
        /// @returns All triggers in the level.
        std::vector<Trigger*> triggers() const;

        // Determine whether the specified ray hits any of the triangles in any of the room geometry.
        // position: The world space position of the source of the ray.
        // direction: The direction of the ray.
        // Returns: The result of the operation. If 'hit' is true, distance and position contain
        // how far along the ray the hit was and the position in world space. The room that was hit
        // is also specified.
        PickResult pick(const ICamera& camera, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;

        /// Render the current scene.
        /// @param device The graphics device to use to render the scene.
        /// @param camera The current camera.
        /// @param render_selection Whether to render selection highlights on selected items.
        void render(const graphics::Device& device, const ICamera& camera, bool render_selection);

        /// Render the transparent triangles in the scene.
        /// @param device The graphics device to use to render the scene.
        /// @param camera The current camera.
        void render_transparency(const graphics::Device& device, const ICamera& camera);

        void set_highlight_mode(RoomHighlightMode mode, bool enabled);
        bool highlight_mode_enabled(RoomHighlightMode mode) const;
        void set_selected_room(uint16_t index);
        void set_selected_item(uint32_t index);
        void set_neighbour_depth(uint32_t depth);
        void on_camera_moved();

        // Set whether to render the alternate mode (the flipmap) or the regular room.
        // enabled: Whether to render the flipmap.
        void set_alternate_mode(bool enabled);

        /// Set whether to render the alternate group specified.
        /// @param group The group to toggle.
        /// @param enabled Whether the group is enabled.
        void set_alternate_group(uint32_t group, bool enabled);

        /// Gets whether the specified alternate group is active.
        /// @param group The group to check.
        /// @returns True if the group is active.
        bool alternate_group(uint32_t group) const;

        // Event raised when the level needs to change the selected room.
        Event<uint16_t> on_room_selected;

        // Event raised when the level needs to change the alternate mode.
        Event<bool> on_alternate_mode_selected;   

        /// Event raised when the levle needs to change the alternate group mode.
        Event<uint16_t, bool> on_alternate_group_selected;

        // Returns the room with ID provided 
        inline Room *room(std::size_t id) const { return _rooms.at(id).get(); }

        // Get the current state of the alternate mode (flipmap).
        bool alternate_mode() const;

        /// Determines if there are any flipmaps in the level.
        /// @returns True if there are flipmaps.
        bool any_alternates() const;

        void set_show_triggers(bool show);

        void set_show_hidden_geometry(bool show);

        bool show_hidden_geometry() const;

        void set_show_water(bool show);

        bool show_triggers() const;

        void set_selected_trigger(uint32_t number);

        const ILevelTextureStorage& texture_storage() const;

        /// Gets the alternate groups that exist in the level.
        /// @returns The set of alternate groups in the level.
        std::set<uint32_t> alternate_groups() const;

        /// Event raised when something has changed in the appearance of the level or the
        /// items that are contained within.
        Event<> on_level_changed;

        trlevel::LevelVersion version() const;
    private:
        void generate_rooms(const graphics::Device& device, const trlevel::ILevel& level);
        void generate_triggers();
        void generate_entities(const graphics::Device& device, const trlevel::ILevel& level, const ITypeNameLookup& type_names);
        void regenerate_neighbours();
        void generate_neighbours(std::set<uint16_t>& results, uint16_t selected_room, int32_t max_depth);

        // Render the rooms in the level.
        // context: The device context.
        // camera: The current camera to render the level with.
        void render_rooms(const graphics::Device& device, const ICamera& camera);

        void render_selected_item(const graphics::Device& device, const ICamera& camera);

        struct RoomToRender
        {
            RoomToRender(Room& room, Room::SelectionMode selection_mode, uint16_t number)
                : room(room), selection_mode(selection_mode), number(number)
            {
            }

            Room&               room;
            Room::SelectionMode selection_mode;
            uint16_t            number;
        };

        // Get the collection of rooms that need to be renderered depending on the current view mode.
        // Returns: The rooms to render and their selection mode.
        std::vector<RoomToRender> get_rooms_to_render(const ICamera& camera) const;

        // Determines whether the room is currently being rendered.
        // room: The room index.
        // Returns: True if the room is visible.
        bool room_visible(uint32_t room) const;

        // Determines whether the alternate mode specified is a mismatch with the current setting of 
        // the alternate mode flag.
        bool is_alternate_mismatch(const Room& room) const;

        bool is_alternate_group_set(uint16_t group) const;

        std::vector<std::unique_ptr<Room>>   _rooms;
        std::vector<std::unique_ptr<Trigger>> _triggers;
        std::vector<std::unique_ptr<Entity>> _entities;
        std::vector<Item> _items;

        graphics::IShader*          _vertex_shader;
        graphics::IShader*          _pixel_shader;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;

        std::set<RoomHighlightMode> _room_highlight_modes;
        
        uint16_t           _selected_room{ 0u };
        Entity*            _selected_item{ nullptr };
        Trigger*           _selected_trigger{ nullptr };
        uint32_t           _neighbour_depth{ 1 };
        std::set<uint16_t> _neighbours;

        std::unique_ptr<ILevelTextureStorage> _texture_storage;
        std::unique_ptr<IMeshStorage> _mesh_storage;
        std::unique_ptr<TransparencyBuffer> _transparency;

        bool _regenerate_transparency{ true };
        bool _alternate_mode{ false };
        bool _show_triggers{ true };
        bool _show_hidden_geometry{ false };
        bool _show_water{ true };

        std::unique_ptr<SelectionRenderer> _selection_renderer;
        std::set<uint32_t> _alternate_groups;
        trlevel::LevelVersion _version;
    };

    /// Find the first item with the type id specified.
    /// @param level The level to search.
    /// @param type_id The type id to search for.
    /// @param output_item The item to output the result into.
    /// @returns True if the item was found.
    bool find_item_by_type_id(const Level& level, uint32_t type_id, Item& output_item);
}
