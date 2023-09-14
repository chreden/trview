#pragma once

#include <map>
#include <variant>
#include <SimpleMath.h>
#include <trview.common/Colour.h>
#include <trview.app/Geometry/IRenderable.h>

namespace trview
{
    struct IRoute;
    struct IItem;
    struct ITrigger;

    /// <summary>
    /// A waypoint in a route.
    /// </summary>
    struct IWaypoint : public IRenderable
    {
        /// <summary>
        /// Defines the possible types of waypoint.
        /// </summary>
        enum class Type
        {
            /// <summary>
            /// The waypoint is a position in the world.
            /// </summary>
            Position,
            /// <summary>
            /// The entity is an entity in the world.
            /// </summary>
            Entity,
            /// <summary>
            /// The waypoint is a trigger in the world.
            /// </summary>
            Trigger
        };

        using WaypointRandomizerSettings = std::map<std::string, std::variant<bool, float, std::string>>;

        /// <summary>
        /// Create a waypoint.
        /// </summary>
        using Source = std::function<std::shared_ptr<IWaypoint>(const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, Type, uint32_t, const Colour&, const Colour&)>;

        /// <summary>
        /// Destructor for IWaypoint.
        /// </summary>
        virtual ~IWaypoint() = 0;
        /// <summary>
        /// Get the bounding box for the waypoint.
        /// </summary>
        /// <returns>The bounding box.</returns>
        virtual DirectX::BoundingBox bounding_box() const = 0;
        /// <summary>
        /// Get the position of the waypoint in the 3D view,
        /// </summary>
        /// <returns></returns>
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        /// <summary>
        /// Get the type of the waypoint.
        /// </summary>
        virtual Type type() const = 0;
        /// <summary>
        /// Get whether the waypoint has an attached save file.
        /// </summary>
        virtual bool has_save() const = 0;
        /// <summary>
        /// Gets the index of the entity or trigger that the waypoint refers to.
        /// </summary>
        virtual uint32_t index() const = 0;
        virtual std::weak_ptr<IItem> item() const = 0;
        /// <summary>
        /// Get any notes associated with the waypoint.
        /// </summary>
        virtual std::string notes() const = 0;
        /// <summary>
        /// Get the room number that the waypoint is in.
        /// </summary>
        virtual uint32_t room() const = 0;
        virtual std::weak_ptr<IRoute> route() const = 0;
        /// <summary>
        /// Render the join between this waypoint and another.
        /// </summary>
        /// <param name="next_waypoint">The waypoint to join to.</param>
        /// <param name="camera">The camera to use to render.</param>
        /// <param name="texture_storage">The texture storage to use to render.</param>
        /// <param name="colour">The colour for the join.</param>
        virtual void render_join(const IWaypoint& next_waypoint, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) = 0;
        /// <summary>
        /// Get the contents of the attached save file.
        /// </summary>
        virtual std::vector<uint8_t> save_file() const = 0;
        virtual void set_item(const std::weak_ptr<IItem>& item) = 0;
        /// Set the notes associated with the waypoint.
        /// @param notes The notes to save.
        virtual void set_notes(const std::string& notes) = 0;
        virtual void set_route(const std::weak_ptr<IRoute>& route) = 0;
        /// Set the route colour for the waypoint blob.
        /// @param colour The colour of the route.
        virtual void set_route_colour(const Colour& colour) = 0;
        /// Set the contents of the attached save file.
        virtual void set_save_file(const std::vector<uint8_t>& data) = 0;
        virtual void set_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;
        /// <summary>
        /// Set the colour for the waypoint stick.
        /// </summary>
        virtual void set_waypoint_colour(const Colour& colour) = 0;
        virtual std::weak_ptr<ITrigger> trigger() const = 0;
        /// <summary>
        /// Get the position of the blob on top of the waypoint pole for rendering.
        /// </summary>
        virtual DirectX::SimpleMath::Vector3 blob_position() const = 0;
        /// <summary>
        /// Get the normal to which the waypoint is aligned.
        /// </summary>
        virtual DirectX::SimpleMath::Vector3 normal() const = 0;
        virtual WaypointRandomizerSettings randomizer_settings() const = 0;
        virtual void set_randomizer_settings(const WaypointRandomizerSettings& settings) = 0;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) = 0;
        virtual Colour route_colour() const = 0;
        virtual Colour waypoint_colour() const = 0;
        virtual void set_normal(const DirectX::SimpleMath::Vector3& normal) = 0;
        virtual void set_room_number(uint32_t room) = 0;
        virtual DirectX::SimpleMath::Vector2 screen_position() const = 0;

        Event<> on_changed;
    };

    /// <summary>
    /// Convert a string value into an IWaypoint::Type.
    /// </summary>
    /// <param name="value">The string to convert.</param>
    /// <returns>The converted type.</returns>
    IWaypoint::Type waypoint_type_from_string(const std::string& value);
    /// <summary>
    /// Convert an IWaypoint::Type to a string.
    /// </summary>
    /// <param name="type">The type to convert.</param>
    /// <returns>The string value.</returns>
    std::string waypoint_type_to_string(IWaypoint::Type type);
}
