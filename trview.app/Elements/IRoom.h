#pragma once

#include <cstdint>
#include <functional>
#include <set>
#include <vector>
#include <SimpleMath.h>
#include <trview.app/Elements/IEntity.h>
#include <trview.app/Elements/ISector.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.app/Elements/RoomInfo.h>
#include <trview.app/Elements/PickFilter.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>
#include <trview.app/Geometry/PickInfo.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Graphics/IMeshStorage.h>

namespace trview
{
    struct ILevel;

    /// <summary>
    /// Represents a room in a level.
    /// </summary>
    struct IRoom
    {
        /// <summary>
        /// The different types of alternate modes for a <see cref="IRoom"/>.
        /// </summary>
        enum class AlternateMode
        {
            /// <summary>
            /// This room does is not an alternate of another room and does not have an alternate room.
            /// </summary>
            None,
            /// <summary>
            /// The room has an alternate room.
            /// </summary>
            HasAlternate,
            /// <summary>
            /// This room is an alternate room to another room.
            /// Note that this won't be set at room creation as it requires an extra processing pass once all rooms are loaded.
            /// </summary>
            IsAlternate
        };

        /// <summary>
        /// Selection modes for the <see cref="IRoom"/> used for rendering.
        /// </summary>
        enum class SelectionMode
        {
            /// <summary>
            /// The room is selected and should be highlighted.
            /// </summary>
            Selected,
            /// <summary>
            /// The room is not selected so should be darkened.
            /// </summary>
            NotSelected
        };

        /// <summary>
        /// Create a new implementation of <see cref="IRoom"/>.
        /// </summary>
        using Source = std::function<std::shared_ptr<IRoom>(const trlevel::ILevel&, const trlevel::tr3_room&,
            const std::shared_ptr<ILevelTextureStorage>&, const IMeshStorage&, uint32_t, const ILevel&)>;
        /// <summary>
        /// Destructor for <see cref="IRoom"/>.
        /// </summary>
        virtual ~IRoom() = 0;
        /// <summary>
        /// Add the specified <see cref="IEntity"/> to the room. The room will not take ownership of this entity.
        /// </summary>
        /// <param name="entity">The <see cref="IEntity"/> to add.</param>
        virtual void add_entity(const std::weak_ptr<IEntity>& entity) = 0;
        /// <summary>
        /// Add the specified <see cref="ITrigger"/> to the room. The room will not take ownership of this trigger.
        /// </summary>
        /// <param name="trigger">The <see cref="ITrigger"/> to add.</param>
        virtual void add_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;
        /// <summary>
        /// Gets the alternate group number for the room. A value of -1 indicates that the room is not part of an alternate group.
        /// </summary>
        /// <returns>The alternate group number of the room.</returns>
        virtual int16_t alternate_group() const = 0;
        /// <summary>
        /// Gets the alternate mode for the room.
        /// </summary>
        /// <returns>The alternate mode for the room</returns>
        virtual AlternateMode alternate_mode() const = 0;
        /// <summary>
        /// Gets the alternate room for this room. If this room does not have an alternate room the value will be -1.
        /// </summary>
        /// <returns>The alternate room number.</returns>
        virtual int16_t alternate_room() const = 0;
        /// <summary>
        /// Gets the bounding box of the room. The bounding box is pre-transformed to the coordinates of the room.
        /// </summary>
        /// <returns>The transformed room bounding box.</returns>
        virtual DirectX::BoundingBox bounding_box() const = 0;
        /// <summary>
        /// Get the centre point of the room based on the room info. This doesn't take into account the contents of the room.
        /// </summary>
        /// <returns>The centre point of the room.</returns>
        virtual DirectX::SimpleMath::Vector3 centre() const = 0;
        /// <summary>
        /// Generate the trigger geometry based on the triggers in the room. This should be called when all adjacent rooms have been
        /// created so that continguous trigger areas can have redundant sides removed.
        /// </summary>
        virtual void generate_trigger_geometry() = 0;
        /// <summary>
        /// Add the transparent triangles for the contents of the room.
        /// </summary>
        /// <param name="transparency">The buffer to which to add triangles.</param>
        /// <param name="camera">The current viewpoint.</param>
        /// <param name="selected">The selection mode for the room.</param>
        /// <param name="show_water">Whether to render water effects.</param>
        virtual void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool show_water) = 0;
        /// <summary>
        /// Get the transparent triangles for the room geometry.
        /// </summary>
        /// <param name="transparency">The buffer to which to add triangles to.</param>
        /// <param name="camera">The current viewpoint.</param>
        /// <param name="selected">The selection mode for the room.</param>
        /// <param name="include_triggers">Whether to include triggers in the output.</param>
        /// <param name="show_water">Whether to render water effects.</param>
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool include_triggers, bool show_water) = 0;
        /// <summary>
        /// Get the room info. This contains basic raw positional information about the room.
        /// </summary>
        /// <returns>The room info.</returns>
        virtual RoomInfo info() const = 0;
        /// <summary>
        /// Get the neighbours of this room. This a set of room numbers that border this room based on sector information.
        /// </summary>
        /// <returns>The neighbours of the room.</returns>
        virtual std::set<uint16_t> neighbours() const = 0;
        /// <summary>
        /// Gets the X dimension of the room measured in sectors.
        /// </summary>
        /// <returns>The X dimension of the room.</returns>
        virtual uint16_t num_x_sectors() const = 0;
        /// <summary>
        /// Gets the Z dimension of the room measured in sectors.
        /// </summary>
        /// <returns>The Z dimension of the room.</returns>
        virtual uint16_t num_z_sectors() const = 0;
        /// <summary>
        /// Get the room number.
        /// </summary>
        /// <returns>The room number.</returns>
        virtual uint32_t number() const = 0;
        /// <summary>
        /// Gets whether the room is an outside room based on the room flags.
        /// </summary>
        /// <returns>Whether the room is an outside room.</returns>
        virtual bool outside() const = 0;
        /// <summary>
        /// Determine whether the specified ray hits any of the triangles in the room geometry.
        /// </summary>
        /// <param name="position">The world space position of the source of the ray.</param>
        /// <param name="direction">The direction of the ray.</param>
        /// <param name="filters">The types of objects to include the picking operation.</param>
        /// <returns>The <see cref="PickResult"/>.</returns>
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, PickFilter filters = PickFilter::Default) const = 0;
        /// <summary>
        /// Gets whether the room is a quicksand room based on the room flags. This can only be true if the game is TR3 or later.
        /// </summary>
        /// <returns>Whether the room is a quicksand room.</returns>
        virtual bool quicksand() const = 0;
        /// <summary>
        /// Render the room.
        /// </summary>
        /// <param name="camera">The current viewpoint.</param>
        /// <param name="selected">Whether the room is selected.</param>
        /// <param name="show_hidden_geometry">Whether to render hidden geometry.</param>
        /// <param name="show_water">Whether to render water effects.</param>
        virtual void render(const ICamera& camera, SelectionMode selected, bool show_hidden_geometry, bool show_water) = 0;
        /// <summary>
        /// Render the contents of the room.
        /// </summary>
        /// <param name="camera">The current viewpoint.</param>
        /// <param name="selected">Whether the room is selected.</param>
        /// <param name="show_water">Whether to render water effects.</param>
        virtual void render_contained(const ICamera& camera, SelectionMode selected, bool show_water) = 0;
        /// <summary>
        /// Gets all sectors in the room.
        /// </summary>
        /// <returns>The sectors in the room.</returns>
        virtual const std::vector<std::shared_ptr<ISector>> sectors() const = 0;
        /// <summary>
        /// Set this room to be the alternate room of the room specified. This will change the alternate mode of this room to IsAlternate.
        /// </summary>
        /// <param name="number">The alternate room number.</param>
        virtual void set_is_alternate(int16_t number) = 0;
        /// <summary>
        /// Get the trigger at the sector coordinates, if any.
        /// </summary>
        /// <param name="x">The sector X coordinate.</param>
        /// <param name="z">The sector Z coordinate.</param>
        /// <returns>The <see cref="ITrigger"/> if found, otherwise empty.</returns>
        virtual std::weak_ptr<ITrigger> trigger_at(int32_t x, int32_t z) const = 0;
        /// <summary>
        /// Update the bounding box based on the geometry and the entities that are in the room.
        /// </summary>
        virtual void update_bounding_box() = 0;
        /// <summary>
        /// Gets whether the room is a water room based on the room flags.
        /// </summary>
        /// <returns>Whether the room is a water room.</returns>
        virtual bool water() const = 0;
    };
}

