#pragma once

#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Graphics/IMeshStorage.h>
#include <trview.app/Elements/Sector.h>
#include <trview.app/Elements/RoomInfo.h>
#include <trview.app/Elements/IEntity.h>
#include <trview.app/Elements/ITrigger.h>

namespace trview
{
    struct ILevel;

    struct IRoom
    {
        enum class AlternateMode
        {
            // This room does not have an alternate room and is not an alternate room to another room.
            None,
            // This room has an alternate room.
            HasAlternate,
            // This room is an alternate room to another room.
            IsAlternate
        };

        enum class SelectionMode
        {
            Selected,
            NotSelected
        };

        using Source = std::function<std::shared_ptr<IRoom>(const trlevel::ILevel&, const trlevel::tr3_room&,
            const ILevelTextureStorage&, const IMeshStorage&, uint32_t, const ILevel&)>;

        virtual ~IRoom() = 0;
        
        // Add the specified entity to the room.
        // Entity: The entity to add.
        virtual void add_entity(IEntity* entity) = 0;
        /// Add the specified trigger to the room.
        /// @paramt trigger The trigger to add.
        virtual void add_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;
        /// Gets the alternate group for the room.
        /// @returns The alternate group number.
        virtual int16_t alternate_group() const = 0;
        /// Determines the alternate state of the room.
        virtual AlternateMode alternate_mode() const = 0;
        /// Gets the room number of the room that is the alternate to this room.
        /// If this room does not have an alternate this will be -1.
        /// @returns The room number of the alternate room.
        virtual int16_t alternate_room() const = 0;
        /// Get the bounding box of the room. The bounding box is pre-transformed to the coordinates of the room.
        /// @returns The bounding box for the room.
        virtual DirectX::BoundingBox bounding_box() const = 0;
        /// Get the centre point of the room.
        /// @returns The centre position of the room.
        virtual DirectX::SimpleMath::Vector3 centre() const = 0;
        virtual void generate_trigger_geometry() = 0;
        /// Add the transparent triangles for entities that are contained inside this room. This is called automatically
        /// if get_transparent_triangles is used.
        /// @param transparency The buffer to add triangles to.
        /// @param camera The current viewpoint.
        /// @param selected The current selection mode.
        virtual void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool show_water, bool force_water = false) = 0;
        /// Add the transparent triangles to the specified transparency buffer.
        /// @param transparency The buffer to add triangles to.
        /// @param camera The current viewpoint.
        /// @param selected The current selection mode.
        /// @param include_triggers Whether to render triggers.
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool include_triggers, bool show_water) = 0;
        virtual RoomInfo info() const = 0;
        virtual std::set<uint16_t> neighbours() const = 0;
        // Returns the number of x sectors in the room 
        virtual uint16_t num_x_sectors() const = 0;
        // Returns the number of z sectors in the room 
        virtual uint16_t num_z_sectors() const = 0;
        virtual uint32_t number() const = 0;
        virtual bool outside() const = 0;
        /// Determine whether the specified ray hits any of the triangles in the room geometry.
        /// @param position The world space position of the source of the ray.
        /// @param direction The direction of the ray.
        /// @returns: The result of the operation. If 'hit' is true, distance and position contain how far along the ray the hit was and the position in world space.
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, bool include_entities, bool include_triggers, bool include_hidden_geometry = false, bool include_room_geometry = true) const = 0;
        virtual bool quicksand() const = 0;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected, bool show_hidden_geometry, bool show_water) = 0;
        virtual void render_contained(const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected, bool show_water, bool force_water = false) = 0;
        /// Returns all sectors 
        virtual const std::vector<std::shared_ptr<ISector>> sectors() const = 0;
        /// Set this room to be the alternate room of the room specified.
        /// This will change the alternate_mode of this room to IsAlternate.
        /// @param number The room number.
        virtual void set_is_alternate(int16_t number) = 0;
        virtual std::weak_ptr<ITrigger> trigger_at(int32_t x, int32_t z) const = 0;
        virtual void update_bounding_box() = 0;
        virtual bool water() const = 0;
    };
}

