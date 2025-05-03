#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "ISoundStorage.h"
#include "ISound.h"

namespace trview
{
    class SoundStorage final : public ISoundStorage
    {
    public:
        explicit SoundStorage(const ISound::Source& sound_source);
        virtual ~SoundStorage() = default;
        void add(Index index, const std::vector<uint8_t>& data) override;
        std::weak_ptr<ISound> get(uint16_t index) const override;
        std::vector<Entry> sounds() const override;
    private:
        struct OwningEntry
        {
            Index index;
            std::shared_ptr<ISound> sound;
        };

        ISound::Source _sound_source;
        std::vector<OwningEntry> _sounds;
    };
}
