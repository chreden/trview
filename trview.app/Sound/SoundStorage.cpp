#include "SoundStorage.h"

namespace trview
{
    ISoundStorage::~ISoundStorage()
    {
    }

    SoundStorage::SoundStorage(const ISound::Source& sound_source)
        : _sound_source(sound_source)
    {
    }

    void SoundStorage::add(uint16_t index, const std::vector<uint8_t>& data)
    {
        const auto sound = _sound_source(data);
        _sounds[index] = sound;
    }

    std::weak_ptr<ISound> SoundStorage::get(uint16_t index) const
    {
        const auto found = _sounds.find(index);
        return found == _sounds.end() ? nullptr : found->second;
    }
}
