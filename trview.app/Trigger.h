#pragma once

#include <cstdint>
#include <vector>

namespace trview
{
    struct TriggerInfo;

    class Trigger final 
    {
    public:
        explicit Trigger(uint32_t number, uint16_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info);

        uint32_t number() const;
        uint16_t room() const;
        uint16_t x() const;
        uint16_t z() const;
        bool triggers_item(uint16_t index) const;
    private:
        std::vector<uint16_t> _objects;
        uint32_t _number;
        uint16_t _room;
        uint16_t _x;
        uint16_t _z;
    };
}