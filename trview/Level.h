#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <unordered_map>
#include <SimpleMath.h>

#include <trview.graphics/Texture.h>
#include <trview.common/Event.h>
#include <trlevel/ILevel.h>

#include "Room.h"
#include "Entity.h"
#include "Mesh.h"
#include "StaticMesh.h"
#include <trview.app/Item.h>
#include <trview.app/Trigger.h>

#include "IMeshStorage.h"

#include <trview.graphics/RenderTarget.h>

namespace trview
{
    struct ILevelTextureStorage;
    struct ICamera;
    class SelectionRenderer;

    namespace graphics
    {
        struct IShaderStorage;
        struct IShader;
    }

    class Level
    {
    public:
        Level(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, const trlevel::ILevel* level);
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

        /// Get the triggers in this level.
        /// @returns All triggers in the level.
        const std::vector<Trigger>& triggers() const;

        // Determine whether the specified ray hits any of the triangles in any of the room geometry.
        // position: The world space position of the source of the ray.
        // direction: The direction of the ray.
        // Returns: The result of the operation. If 'hit' is true, distance and position contain
        // how far along the ray the hit was and the position in world space. The room that was hit
        // is also specified.
        PickResult pick(const ICamera& camera, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;

        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera);

        RoomHighlightMode highlight_mode() const;
        void set_highlight_mode(RoomHighlightMode mode);
        void set_selected_room(uint16_t index);
        void set_selected_item(uint16_t index);
        void set_neighbour_depth(uint32_t depth);
        void on_camera_moved();

        // Set whether to render the alternate mode (the flipmap) or the regular room.
        // enabled: Whether to render the flipmap.
        void set_alternate_mode(bool enabled);

        // Event raised when the level needs to change the selected room.
        Event<uint16_t> on_room_selected;

        // Event raised when the level needs to change the alternate mode.
        Event<bool> on_alternate_mode_selected;   

        // Returns the room with ID provided 
        inline Room *room(std::size_t id) const { return _rooms.at(id).get(); }

        // Get the current state of the alternate mode (flipmap).
        bool alternate_mode() const;

        /// Determines if there are any flipmaps in the level.
        /// @returns True if there are flipmaps.
        bool any_alternates() const;

        void set_show_triggers(bool show);

        bool show_triggers() const;
    private:
        void generate_rooms(const Microsoft::WRL::ComPtr<ID3D11Device>& device);
        void generate_triggers();
        void generate_entities(const Microsoft::WRL::ComPtr<ID3D11Device>& device);
        void regenerate_neighbours();
        void generate_neighbours(std::set<uint16_t>& all_rooms, uint16_t previous_room, uint16_t selected_room, int32_t current_depth, int32_t max_depth);

        // Render the rooms in the level.
        // context: The device context.
        // camera: The current camera to render the level with.
        void render_rooms(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera);

        void render_selected_item(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera);

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
        bool is_alternate_mismatch(Room::AlternateMode mode) const;

        /// Load the type name lookup table from resources.
        void load_type_name_lookup();

        /// Look up the type name of the specified type id.
        /// @param type_id The type id to look up.
        /// @returns The type name of the item.
        /// @remarks If the type id is not found, this will return the string version of the type id.
        std::wstring lookup_type_name(uint32_t type_id) const;

        const trlevel::ILevel*               _level;
        std::vector<std::unique_ptr<Room>>   _rooms;
        std::vector<Trigger>                 _triggers;
        std::vector<std::unique_ptr<Entity>> _entities;
        std::vector<Item> _items;

        graphics::IShader*          _vertex_shader;
        graphics::IShader*          _pixel_shader;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;

        RoomHighlightMode  _room_highlight_mode{ RoomHighlightMode::None };
        uint16_t           _selected_room{ 0u };
        Entity*            _selected_item{ nullptr };
        uint32_t           _neighbour_depth{ 1 };
        std::set<uint16_t> _neighbours;

        std::unique_ptr<ILevelTextureStorage> _texture_storage;
        std::unique_ptr<IMeshStorage> _mesh_storage;
        std::unique_ptr<TransparencyBuffer> _transparency;

        bool _regenerate_transparency{ true };
        bool _alternate_mode{ false };
        bool _show_triggers{ true };

        std::unordered_map<uint32_t, std::wstring> _type_names;

        std::unique_ptr<SelectionRenderer> _selection_renderer;
    };
}
