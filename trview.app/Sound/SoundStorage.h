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
        void add(uint16_t index, const std::vector<uint8_t>& data) override;
        std::weak_ptr<ISound> get(uint16_t index) const override;
    private:
        ISound::Source _sound_source;
        std::unordered_map<uint16_t, std::shared_ptr<ISound>> _sounds;
    };
}
