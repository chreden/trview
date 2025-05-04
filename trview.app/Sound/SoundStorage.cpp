#include "SoundStorage.h"
#include <ranges>

namespace trview
{
    ISoundStorage::~ISoundStorage()
    {
    }

    SoundStorage::SoundStorage(const ISound::Source& sound_source)
        : _sound_source(sound_source)
    {
    }

    void SoundStorage::add(Index index, const std::vector<uint8_t>& data)
    {
        const auto sound = _sound_source(data);
        _sounds.push_back({ .index = index, .sound = sound });
    }

    std::weak_ptr<ISound> SoundStorage::get(uint16_t index) const
    {
        const auto found = std::ranges::find_if(_sounds, [=](auto& s) { return s.index.sample_index == index; });
        return found == _sounds.end() ? nullptr : found->sound;
    }

    std::vector<ISoundStorage::Entry> SoundStorage::sounds() const
    {
        return _sounds | std::views::transform([](auto&& e) -> Entry { return { .index = e.index, .sound = e.sound }; }) | std::ranges::to<std::vector>();
    }
}
