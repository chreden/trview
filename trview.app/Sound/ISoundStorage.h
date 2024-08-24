#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace trview
{
    struct ISound;

    struct ISoundStorage
    {
        virtual ~ISoundStorage() = 0;
        virtual void add(uint16_t index, const std::vector<uint8_t>& data) = 0;
        virtual std::weak_ptr<ISound> get(uint16_t index) const = 0;
        virtual std::unordered_map<uint16_t, std::weak_ptr<ISound>> sounds() const = 0;
    };
}
