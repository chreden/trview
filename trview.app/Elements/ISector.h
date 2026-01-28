#pragma once

#include <array>
#include <set>
#include <unordered_set>
#include <vector>
#include <SimpleMath.h>
#include "Types.h"
#include "trlevel/ILevel.h"
#include "trlevel/trtypes.h"
#include "Floordata.h"

#include "../Filters/IFilterable.h"

namespace trview
{
    struct IRoom;
    struct ITrigger;

    struct ISector : public IFilterable
    {
        using Source = std::function<std::shared_ptr<ISector>(const trlevel::ILevel&, const trlevel::tr3_room&,
            const trlevel::tr_room_sector&, int, const std::weak_ptr<IRoom>&, uint32_t)>;

        enum class Corner
        {
            SW,
            NW,
            SE,
            NE
        };

        struct Triangle
        {
            Triangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2,
                const DirectX::SimpleMath::Vector2& uv0, const DirectX::SimpleMath::Vector2& uv1, const DirectX::SimpleMath::Vector2& uv2,
                SectorFlag type, uint32_t room);

            Triangle(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2, SectorFlag type, uint32_t room);

            DirectX::SimpleMath::Vector3 v0;
            DirectX::SimpleMath::Vector3 v1;
            DirectX::SimpleMath::Vector3 v2;
            DirectX::SimpleMath::Vector2 uv0;
            DirectX::SimpleMath::Vector2 uv1;
            DirectX::SimpleMath::Vector2 uv2;
            DirectX::SimpleMath::Vector3 normal;
            SectorFlag type;
            uint32_t room;

            Triangle operator+(const DirectX::SimpleMath::Vector3& offset) const;
            bool operator==(const Triangle& other) const;
        };

        struct Quad
        {
            Quad(const DirectX::SimpleMath::Vector3& v0, const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2, const DirectX::SimpleMath::Vector3& v3, SectorFlag type, uint32_t room);

            std::array<DirectX::SimpleMath::Vector3, 4> v;
            std::array<DirectX::SimpleMath::Vector2, 4> uv;
            SectorFlag type;
            uint32_t room;

            float min;
            float max;
            float height;

            std::vector<Triangle> triangles();
        };

        struct Portal
        {
            /// <summary>
            /// The direct sector - not following any portals.
            /// </summary>
            std::shared_ptr<ISector> direct;
            std::shared_ptr<IRoom> direct_room;
            /// <summary>
            /// The sector that the portal leads to.
            /// </summary>
            std::shared_ptr<ISector> target;
            /// <summary>
            /// The offset to add to values to put it into the space of the original sector.
            /// </summary>
            DirectX::SimpleMath::Vector3 offset;
            DirectX::SimpleMath::Vector3 above_offset;
            std::shared_ptr<ISector> sector_above;
            std::shared_ptr<IRoom> room_above;
            DirectX::SimpleMath::Vector3 below_offset;
            std::shared_ptr<ISector> sector_below;
            std::shared_ptr<IRoom> room_below;

            SectorFlag flags() const;
            DirectX::SimpleMath::Vector3 corner(Corner corner) const;
            DirectX::SimpleMath::Vector3 ceiling(Corner corner) const;
            operator bool() const;
            bool is_wall() const;
            bool is_portal() const;
        };

        virtual ~ISector() = 0;
        virtual std::vector<std::uint16_t> portals() const = 0;
        virtual int id() const = 0;
        virtual std::set<std::uint16_t> neighbours() const = 0;
        virtual std::uint16_t room_below() const = 0;
        virtual std::uint16_t room_above() const = 0;
        virtual SectorFlag flags() const = 0;
        virtual TriggerInfo trigger_info() const = 0;
        virtual uint16_t x() const = 0;
        virtual uint16_t z() const = 0;
        virtual std::array<float, 4> corners() const = 0;
        virtual std::array<float, 4> ceiling_corners() const = 0;
        virtual DirectX::SimpleMath::Vector3 corner(Corner corner) const = 0;
        virtual int8_t ceiling() const = 0;
        virtual DirectX::SimpleMath::Vector3 ceiling(Corner corner) const = 0;
        virtual std::weak_ptr<IRoom> room() const = 0;
        virtual int8_t tilt_x() const = 0;
        virtual int8_t tilt_z() const = 0;
        virtual TriangulationDirection triangulation() const = 0;
        virtual std::vector<Triangle> triangles() const = 0;
        virtual bool is_floor() const = 0;
        virtual bool is_wall() const = 0;
        virtual uint32_t floordata_index() const = 0;
        virtual bool is_portal() const = 0;
        virtual bool is_ceiling() const = 0;

        // New triangle generation to include TRLE mode.
        virtual void generate_triangles() = 0;
        virtual void add_triangle(const ISector::Portal& portal, const Triangle& triangle, std::unordered_set<uint32_t> visited_rooms) = 0;
        virtual void add_flag(SectorFlag flag) = 0;

        virtual void set_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;
        virtual std::weak_ptr<ITrigger> trigger() const = 0;

        virtual TriangulationDirection ceiling_triangulation() const = 0;
        virtual uint32_t number() const = 0;
        virtual int8_t floor() const = 0;
        virtual uint16_t material() const = 0;
        virtual uint16_t box_index() const = 0;
        virtual bool stopper() const = 0;
    };

    bool is_no_space(SectorFlag flags);
    constexpr std::string to_string(TriangulationDirection direction);
    uint32_t sector_room(const std::shared_ptr<ISector>& sector);
}

#include "ISector.hpp"

