#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <set>

#include <trlevel/ILevel.h>
#include <trview.app/Elements/ILevel.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>
#include <trview.app/Geometry/Mesh.h>
#include <trview.app/Graphics/ISelectionRenderer.h>
#include <trview.app/Graphics/IMeshStorage.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.graphics/Texture.h>

#include "StaticMesh.h"

namespace trview
{
    struct ILevelTextureStorage;
    struct ICamera;
    struct ITypeNameLookup;

    namespace graphics
    {
        struct IShaderStorage;
        struct IShader;
    }

    class Level : public ILevel
    {
    public:
        Level(const std::shared_ptr<graphics::IDevice>& device,
            const std::shared_ptr<graphics::IShaderStorage>& shader_storage,
            std::unique_ptr<trlevel::ILevel>&& level,
            std::unique_ptr<ILevelTextureStorage> level_texture_storage,
            std::unique_ptr<IMeshStorage> mesh_storage,
            std::unique_ptr<ITransparencyBuffer> transparency_buffer,
            std::unique_ptr<ISelectionRenderer> selection_renderer,
            const std::shared_ptr<ITypeNameLookup>& type_names,
            const IMesh::Source& mesh_source,
            const IMesh::TransparentSource& mesh_transparent_source,
            const IEntity::EntitySource& entity_source,
            const IEntity::AiSource ai_source);
        ~Level();

        // Temporary, for the room info and texture window.
        std::vector<RoomInfo> room_info() const;
        RoomInfo room_info(uint32_t room) const;
        std::vector<graphics::Texture> level_textures() const;

        uint16_t selected_room() const;

        /// Get the items in this level.
        /// @returns All items in the level.
        std::vector<Item> items() const;

        /// Get the number of rooms in the level.
        uint32_t number_of_rooms() const;

        std::vector<Room*> rooms() const;

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
        /// @param camera The current camera.
        /// @param render_selection Whether to render selection highlights on selected items.
        void render(const ICamera& camera, bool render_selection);

        /// Render the transparent triangles in the scene.
        /// @param camera The current camera.
        void render_transparency(const ICamera& camera);

        void set_highlight_mode(RoomHighlightMode mode, bool enabled);
        bool highlight_mode_enabled(RoomHighlightMode mode) const;
        void set_selected_room(uint16_t index);
        void set_selected_item(uint32_t index);
        void set_neighbour_depth(uint32_t depth);
        void on_camera_moved();
        void set_item_visibility(uint32_t index, bool state);
        void set_trigger_visibility(uint32_t index, bool state);

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
        void set_show_wireframe(bool show);
        bool show_triggers() const;
        void set_selected_trigger(uint32_t number);

        const ILevelTextureStorage& texture_storage() const;

        /// Gets the alternate groups that exist in the level.
        /// @returns The set of alternate groups in the level.
        std::set<uint32_t> alternate_groups() const;

        trlevel::LevelVersion version() const;

        std::string filename() const;

        void set_filename(const std::string& filename);
    private:
        void generate_rooms(const trlevel::ILevel& level, const IMesh::Source& mesh_source);
        void generate_triggers(const IMesh::TransparentSource& mesh_transparent_source);
        void generate_entities(const trlevel::ILevel& level, const ITypeNameLookup& type_names, const IEntity::EntitySource& entity_source, const IEntity::AiSource& ai_source);
        void regenerate_neighbours();
        void generate_neighbours(std::set<uint16_t>& results, uint16_t selected_room, int32_t max_depth);

        // Render the rooms in the level.
        // context: The device context.
        // camera: The current camera to render the level with.
        void render_rooms(const ICamera& camera);

        void render_selected_item(const ICamera& camera);

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

        std::shared_ptr<graphics::IDevice> _device;
        std::vector<std::unique_ptr<Room>>   _rooms;
        std::vector<std::unique_ptr<Trigger>> _triggers;
        std::vector<std::shared_ptr<IEntity>> _entities;
        std::vector<Item> _items;

        graphics::IShader*          _vertex_shader;
        graphics::IShader*          _pixel_shader;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> _wireframe_rasterizer;


        std::set<RoomHighlightMode> _room_highlight_modes;
        
        uint16_t           _selected_room{ 0u };
        std::weak_ptr<IEntity> _selected_item;
        Trigger*           _selected_trigger{ nullptr };
        uint32_t           _neighbour_depth{ 1 };
        std::set<uint16_t> _neighbours;

        std::unique_ptr<ILevelTextureStorage> _texture_storage;
        std::unique_ptr<IMeshStorage> _mesh_storage;
        std::unique_ptr<ITransparencyBuffer> _transparency;

        bool _regenerate_transparency{ true };
        bool _alternate_mode{ false };
        bool _show_triggers{ true };
        bool _show_hidden_geometry{ false };
        bool _show_water{ true };
        bool _show_wireframe{ false };

        std::unique_ptr<ISelectionRenderer> _selection_renderer;
        std::set<uint32_t> _alternate_groups;
        trlevel::LevelVersion _version;
        std::string _filename;
    };

    /// Find the first item with the type id specified.
    /// @param level The level to search.
    /// @param type_id The type id to search for.
    /// @param output_item The item to output the result into.
    /// @returns True if the item was found.
    bool find_item_by_type_id(const ILevel& level, uint32_t type_id, Item& output_item);
}
