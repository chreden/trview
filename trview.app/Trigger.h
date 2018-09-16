#pragma once

#include <cstdint>
#include <vector>

namespace trview
{
    struct TriggerInfo;

    class Trigger final 
    {
    public:
        Trigger(const TriggerInfo& trigger_info);

        bool triggers_item(uint16_t index) const;
    private:
        std::vector<uint16_t> _objects;
    };
}