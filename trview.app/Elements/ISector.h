#pragma once

#include <array>
#include <set>
#include <vector>
#include <SimpleMath.h>
#include "Types.h"
#include "trlevel/ILevel.h"
#include "trlevel/trtypes.h"

namespace trview
{
    struct ISector
    {
        using Source = std::function<std::shared_ptr<ISector>(const trlevel::ILevel&, const trlevel::tr3_room&,
            const trlevel::tr_room_sector&, int, uint32_t)>;

        enum class TriangulationDirection
        {
            None,
            NwSe,
            NeSw
        };

        virtual ~ISector() = 0;
        virtual std::uint16_t portal() const = 0;
        virtual inline int id() const = 0;
        virtual std::set<std::uint16_t> neighbours() const = 0;
        virtual inline std::uint16_t room_below() const = 0;
        virtual inline std::uint16_t room_above() const = 0;
        virtual std::uint16_t flags() const = 0;
        virtual TriggerInfo trigger() const = 0;
        virtual uint16_t x() const = 0;
        virtual uint16_t z() const = 0;
        virtual std::array<float, 4> corners() const = 0;
        virtual uint32_t room() const = 0;
        virtual TriangulationDirection triangulation_function() const = 0;
        virtual std::vector<DirectX::SimpleMath::Vector3> triangles() const = 0;
        virtual bool is_floor() const = 0;
    };
}
