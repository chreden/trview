#pragma once

#include "ISound.h"

namespace trview
{
    class Sound final : public ISound
    {
    public:
        explicit Sound(const std::vector<uint8_t>& data);
        virtual ~Sound();
        void play() override;
    private:
        bool initialise();
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };
}
