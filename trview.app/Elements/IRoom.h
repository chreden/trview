#pragma once

#include <cstdint>
#include <functional>
#include <set>
#include <vector>
#include <unordered_set>
#include <SimpleMath.h>
#include "IItem.h"
#include <trview.app/Elements/ISector.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.app/Elements/RoomInfo.h>
#include <trview.app/Elements/PickFilter.h>
#include <trview.app/Elements/ILight.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>
#include <trview.app/Geometry/PickInfo.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Graphics/IMeshStorage.h>
#include <trview.common/Logs/Activity.h>
#include "RenderFilter.h"
#include "CameraSink/ICameraSink.h"
#include "IStaticMesh.h"
#include "../Filters/IFilterable.h"

namespace trview
{
    struct ILevel;

    /// <summary>
    /// Represents a room in a level.
    /// </summary>
    struct IRoom : public IFilterable
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

        enum class Flag
        {
            Water = 0x1,
            Bit1 = 0x2,
            Bit2 = 0x4,
            Outside = 0x8,
            Bit4 = 0x10,
            Wind = 0x20,
            Bit6 = 0x40,
            Bit7 = 0x80,
            Quicksand = 0x80,
            NoLensFlare = 0x80,
            Caustics = 0x100,
            WaterReflectivity = 0x200,
            Bit10 = 0x400,
            Bit11 = 0x800,
            Bit12 = 0x1000,
            Bit13 = 0x2000,
            Bit14 = 0x4000,
            Bit15 = 0x8000
        };

        Event<> on_changed;

        /// <summary>
        /// Create a new implementation of <see cref="IRoom"/>.
        /// </summary>
        using Source = std::function<std::shared_ptr<IRoom>(const trlevel::ILevel&, const trlevel::tr3_room&,
            const std::shared_ptr<ILevelTextureStorage>&, const IMeshStorage&, uint32_t, const std::weak_ptr<ILevel>&, uint32_t, const Activity& activity)>;
        /// <summary>
        /// Destructor for <see cref="IRoom"/>.
        /// </summary>
        virtual ~IRoom() = 0;
        /// <summary>
        /// Add the specified <see cref="IEntity"/> to the room. The room will not take ownership of this entity.
        /// </summary>
        /// <param name="entity">The <see cref="IEntity"/> to add.</param>
        virtual void add_entity(const std::weak_ptr<IItem>& entity) = 0;
        /// <summary>
        /// Add the specified <see cref="ITrigger"/> to the room. The room will not take ownership of this trigger.
        /// </summary>
        /// <param name="trigger">The <see cref="ITrigger"/> to add.</param>
        virtual void add_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;
        /// <summary>
        /// Add the specified <see cref="ILight"/> to the room. The room will not take ownership of this light.
        /// </summary>
        /// <param name="light"></param>
        virtual void add_light(const std::weak_ptr<ILight>& light) = 0;
        /// <summary>
        /// Add the specified <see cref="ICameraSink"/> to the room. The room will not take ownership of this camera/sink.
        /// </summary>
        virtual void add_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
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
        /// Gets the ambient colour for this room.
        /// </summary>
        /// <returns>The ambient colour.</returns>
        virtual Colour ambient() const = 0;
        virtual int16_t ambient_intensity_1() const = 0;
        virtual int16_t ambient_intensity_2() const = 0;
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
        /// Calculate the sector triangles. Adjacent rooms will be ready for querying at this point.
        /// </summary>
        virtual void generate_sector_triangles() = 0;
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
        /// <param name="render_filter">What to render.</param>
        virtual void get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, RenderFilter render_filter) = 0;
        /// <summary>
        /// Get the transparent triangles for the room geometry.
        /// </summary>
        /// <param name="transparency">The buffer to which to add triangles to.</param>
        /// <param name="camera">The current viewpoint.</param>
        /// <param name="selected">The selection mode for the room.</param>
        /// <param name="show_items">Whether to render items.</param>
        /// <param name="render_filter">What to render.</param>
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, RenderFilter render_filter) = 0;
        /// <summary>
        /// Checks whether the room has the specified flag.
        /// </summary>
        /// <param name="flag">The flag to check.</param>
        /// <returns>Whether the room has the flag.</returns>
        virtual bool flag(Flag flag) const = 0;
        /// <summary>
        /// All room flags.
        /// </summary>
        virtual uint16_t flags() const = 0;
        /// <summary>
        /// Get the room info. This contains basic raw positional information about the room.
        /// </summary>
        /// <returns>The room info.</returns>
        virtual RoomInfo info() const = 0;
        virtual int16_t light_mode() const = 0;
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
        virtual std::vector<PickResult> pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, PickFilter filters = PickFilter::Default) const = 0;
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
        /// <param name="render_filter">What to render.</param>
        /// <param name="visible_rooms">The rooms that are currently being rendered.</param>
        virtual void render(const ICamera& camera, SelectionMode selected, RenderFilter render_filter, const std::unordered_set<uint32_t>& visible_rooms) = 0;
        /// <summary>
        /// Render the bounding boxes in the room.
        /// </summary>
        /// <param name="camera">The current viewpoint.</param>
        virtual void render_bounding_boxes(const ICamera& camera) = 0;
        /// <summary>
        /// Render the lights in the room.
        /// </summary>
        /// <param name="camera">The current viewpoint.</param>
        virtual void render_lights(const ICamera& camera, const std::weak_ptr<ILight>& selected_light) = 0;

        virtual void render_camera_sinks(const ICamera& camera) = 0;
        /// <summary>
        /// Render the contents of the room.
        /// </summary>
        /// <param name="camera">The current viewpoint.</param>
        /// <param name="selected">Whether the room is selected.</param>
        /// <param name="render_filter">What to render.</param>
        virtual void render_contained(const ICamera& camera, SelectionMode selected, RenderFilter render_filter) = 0;
        virtual std::weak_ptr<ISector> sector(int32_t x, int32_t z) const = 0;
        virtual DirectX::SimpleMath::Vector3 sector_centroid(const std::weak_ptr<ISector>& sector) const = 0;
        /// <summary>
        /// Gets all sectors in the room.
        /// </summary>
        /// <returns>The sectors in the room.</returns>
        virtual std::vector<std::shared_ptr<ISector>> sectors() const = 0;
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
        /// <summary>
        /// Gets the y bottom value of the room.
        /// </summary>
        /// <returns></returns>
        virtual float y_bottom() const = 0;
        /// <summary>
        /// Gets the y top value of the room.
        /// </summary>
        /// <returns></returns>
        virtual float y_top() const = 0;
        /// <summary>
        /// Get sector portal information from one sector to another. This can potentially link to a sector in another room.
        /// </summary>
        /// <param name="x1">X coordinate of current sector.</param>
        /// <param name="z1">Z coordinate of current sector.</param>
        /// <param name="x2">X coordinate of target sector.</param>
        /// <param name="z2">Z coordinate of target sector.</param>
        virtual ISector::Portal sector_portal(int x1, int z1, int x2, int z2) const = 0;
        /// <summary>
        /// Set the rooms for the sector triangles. This should be a collection matching the
        /// sector triangles in the room.
        /// </summary>
        /// <param name="triangles">The sector triangle rooms.</param>
        virtual void set_sector_triangle_rooms(const std::vector<uint32_t>& triangles) = 0;
        /// <summary>
        /// Get the X/Z position of the room. Y is not included.
        /// </summary>
        /// <returns>The X/Z scaled position of the room.</returns>
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        /// <summary>
        /// Gets whether the room is visible.
        /// </summary>
        virtual bool visible() const = 0;
        /// <summary>
        /// Sets whether the room is visible.
        /// </summary>
        virtual void set_visible(bool visible) = 0;
        virtual std::vector<std::weak_ptr<ILight>> lights() const = 0;
        virtual std::vector<std::weak_ptr<ICameraSink>> camera_sinks() const = 0;
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const = 0;
        virtual std::vector<std::weak_ptr<IItem>> items() const = 0;
        virtual std::weak_ptr<ILevel> level() const = 0;
        virtual std::vector<std::weak_ptr<IStaticMesh>> static_meshes() const = 0;
        virtual void update(float delta) = 0;
        virtual uint16_t water_scheme() const = 0;
    };

    /// <summary>
    /// Find the sector in the room that contains the provided point, or nullptr.
    /// </summary>
    /// <param name="room">The room to search.</param>
    /// <param name="point">The point to test.</param>
    /// <returns>The sector or nullptr if no match.</returns>
    std::shared_ptr<ISector> sector_from_point(const IRoom& room, const DirectX::SimpleMath::Vector3& point);

    std::string to_string(IRoom::AlternateMode mode);

    std::string light_mode_name(int16_t light_mode);
    uint32_t room_number(const std::weak_ptr<IRoom>& room);

    struct VerticalPortalInfo
    {
        std::shared_ptr<ISector>        sector;
        DirectX::SimpleMath::Vector3    offset;
        std::shared_ptr<IRoom>          room;
    };

    std::optional<VerticalPortalInfo> sector_above(const std::shared_ptr<ISector>& sector);
    std::optional<VerticalPortalInfo> sector_below(const std::shared_ptr<ISector>& sector);
}

