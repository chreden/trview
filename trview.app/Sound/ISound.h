#pragma once

namespace trview
{
    struct ISound
    {
        using Source = std::function<std::shared_ptr<ISound>(const std::vector<uint8_t>&)>;
        virtual ~ISound() = 0;
        virtual void play() = 0;
    };

    std::shared_ptr<ISound> create_sound(const std::vector<uint8_t>& data);
}
