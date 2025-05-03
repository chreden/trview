#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace trview
{
    struct ISound;

    struct ISoundStorage
    {
        struct Index
        {
            uint16_t sound_map;
            uint16_t sound_details;
            uint16_t sample_index;
            auto operator<=>(const Index&) const = default;
        };

        struct Entry
        {
            Index index;
            std::weak_ptr<ISound> sound;
        };

        virtual ~ISoundStorage() = 0;
        virtual void add(Index index, const std::vector<uint8_t>& data) = 0;
        virtual std::weak_ptr<ISound> get(uint16_t index) const = 0;
        virtual std::vector<Entry> sounds() const = 0;
    };
}
