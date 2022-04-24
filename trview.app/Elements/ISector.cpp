#include "ISector.h"
#include "IRoom.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    ISector::~ISector()
    {
    }

    Vector3 ISector::Portal::corner(ISector::Corner corner) const
    {
        if (has_flag(direct->flags(), SectorFlag::Portal))
        {
            return target->corner(corner) + offset;
        }
        return direct->corner(corner);
    }

    Vector3 ISector::Portal::ceiling(Corner corner) const
    {
        return direct->ceiling(corner);
    }

    SectorFlag ISector::Portal::flags() const
    {
        return direct->flags();
    }

    ISector::Portal::operator bool() const
    {
        return direct != nullptr;
    }

    bool ISector::Portal::is_wall() const
    {
        return direct->is_wall();
    }

    bool ISector::Portal::is_portal() const
    {
        return direct->is_portal();
    }

    ISector::Triangle ISector::Triangle::operator+(const DirectX::SimpleMath::Vector3& offset) const
    {
        return { v0 + offset, v1 + offset, v2 + offset, type, room };
    }

    bool ISector::Triangle::operator==(const Triangle& other) const
    {
        return v0 == other.v0 && v1 == other.v1 && v2 == other.v2;
    }

    ISector::Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2,
        const Vector2& uv0, const Vector2& uv1, const Vector2& uv2,
        SectorFlag type, uint32_t room)
        : v0(v0), v1(v1), v2(v2), uv0(uv0), uv1(uv1), uv2(uv2), type(type), room(room)
    {
    }

    ISector::Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, SectorFlag type, uint32_t room)
        : Triangle(v0, v1, v2, Vector2::Zero, Vector2::Zero, Vector2::Zero, type, room)
    {
    }

    ISector::Quad::Quad(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, SectorFlag type, uint32_t room)
        : v{ v0, v1, v2, v3 }, uv{ Vector2(0, 1), Vector2(1, 0), Vector2(0, 0), Vector2(1, 1) }, type(type), room(room)
    {
        // Calculate a bunch of stats that can be used later.
        min = FLT_MAX;
        max = -FLT_MAX;
        for (const auto& vs : v)
        {
            if (vs.y > max)
            {
                max = vs.y;
            }

            if (vs.y < min)
            {
                min = vs.y;
            }
        }
        height = max - min;
    }

    std::vector<ISector::Triangle> ISector::Quad::triangles()
    {
        std::vector<Triangle> results;
        if (has_flag(type, SectorFlag::Wall) || height > 1)
        {
            uv[0].y = v[0].y;
            uv[1].y = v[1].y;
            uv[2].y = v[2].y;
            uv[3].y = v[3].y;
        }
        results.push_back(Triangle(v[0], v[1], v[2], uv[0], uv[1], uv[2], type, room));
        results.push_back(Triangle(v[0], v[3], v[1], uv[0], uv[3], uv[1], type, room));
        return results;
    }
}
