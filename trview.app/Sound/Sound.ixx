export module trview.app:Sound;

import :ISound;

namespace trview
{
    export class Sound final : public ISound
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
