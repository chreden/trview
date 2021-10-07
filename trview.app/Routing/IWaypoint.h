#pragma once

#include <SimpleMath.h>
#include <trview.common/Colour.h>
#include <trview.app/Geometry/IRenderable.h>

namespace trview
{
    struct IWaypoint : public IRenderable
    {
        /// Defines the type of waypoint.
        enum class Type
        {
            /// The waypoint is to a position in the world.
            Position,
            /// The waypoint is to an entity in the world.
            Entity,
            /// The waypoint is to a trigger in the world.
            Trigger,
            /// <summary>
            /// The waypoint is a Randomizer secret entry.
            /// </summary>
            RandoLocation
        };

        using Source = std::function<std::unique_ptr<IWaypoint>(const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, Type, uint32_t, const Colour&)>;

        virtual ~IWaypoint() = 0;
        /// <summary>
        /// Get the bounding box for the waypoint.
        /// </summary>
        /// <returns>The bounding box.</returns>
        virtual DirectX::BoundingBox bounding_box() const = 0;
        /// Get the position of the waypoint in the 3D view.
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        /// Get the type of the waypoint.
        virtual Type type() const = 0;
        /// Get whether the waypoint has an attached save file.
        virtual bool has_save() const = 0;
        /// Gets the index of the entity or trigger that the waypoint refers to.
        virtual uint32_t index() const = 0;
        /// Gets the room that the waypoint is in.
        virtual uint32_t room() const = 0;
        /// Get any notes associated with the waypoint.
        virtual std::wstring notes() const = 0;
        /// <summary>
        /// Render the join between this waypoint and another.
        /// </summary>
        /// <param name="next_waypoint">The waypoint to join to.</param>
        /// <param name="camera">The camera to use to render.</param>
        /// <param name="texture_storage">The texture storage to use to render.</param>
        /// <param name="colour">The colour for the join.</param>
        virtual void render_join(const IWaypoint& next_waypoint, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) = 0;
        /// Get the contents of the attached save file.
        virtual std::vector<uint8_t> save_file() const = 0;
        /// Set the notes associated with the waypoint.
        /// @param notes The notes to save.
        virtual void set_notes(const std::wstring& notes) = 0;
        /// Set the route colour for the waypoint blob.
        /// @param colour The colour of the route.
        virtual void set_route_colour(const Colour& colour) = 0;
        /// Set the contents of the attached save file.
        virtual void set_save_file(const std::vector<uint8_t>& data) = 0;
        virtual DirectX::SimpleMath::Vector3 blob_position() const = 0;
        virtual DirectX::SimpleMath::Vector3 normal() const = 0;
    };

    IWaypoint::Type waypoint_type_from_string(const std::string& value);
    std::wstring waypoint_type_to_string(IWaypoint::Type type);
}
