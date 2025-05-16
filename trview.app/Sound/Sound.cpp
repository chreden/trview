#include "Sound.h"

#define MINIAUDIO_IMPLEMENTATION
#include <external/miniaudio/miniaudio.h>

namespace trview
{
    namespace
    {
        void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
        {
            ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
            if (pDecoder == nullptr)
            {
                return;
            }

            ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
            (void)pInput;
        }
    }

    ISound::~ISound()
    {
    }

    struct Sound::Impl
    {
        std::vector<uint8_t> data;
        bool initialised{ false };
        bool failed{ false };
        ma_decoder decoder;
        ma_device_config deviceConfig;
        ma_device device;
    };

    Sound::Sound(const std::vector<uint8_t>& data)
        : _impl(std::make_unique<Impl>())
    {
        _impl->data = data;
    }

    Sound::~Sound()
    {
        ma_device_uninit(&_impl->device);
        ma_decoder_uninit(&_impl->decoder);
    }

    void Sound::play()
    {
        if (initialise())
        {
            ma_decoder_seek_to_pcm_frame(&_impl->decoder, 0);
            ma_device_start(&_impl->device);
        }
    }

    bool Sound::initialise()
    {
        if (_impl->initialised)
        {
            return true;
        }

        if (_impl->failed)
        {
            return false;
        }

        if (_impl->data.empty() ||
            MA_SUCCESS != ma_decoder_init_memory(&_impl->data[0], _impl->data.size(), nullptr, &_impl->decoder))
        {
            _impl->failed = true;
            return false;
        }

        _impl->deviceConfig = ma_device_config_init(ma_device_type_playback);
        _impl->deviceConfig.playback.format = _impl->decoder.outputFormat;
        _impl->deviceConfig.playback.channels = _impl->decoder.outputChannels;
        _impl->deviceConfig.sampleRate = _impl->decoder.outputSampleRate;
        _impl->deviceConfig.dataCallback = data_callback;
        _impl->deviceConfig.pUserData = &_impl->decoder;

        if (MA_SUCCESS != ma_device_init(NULL, &_impl->deviceConfig, &_impl->device))
        {
            _impl->failed = true;
            return false;
        }

        _impl->initialised = true;
        return true;
    }

    std::shared_ptr<ISound> create_sound(const std::vector<uint8_t>& data)
    {
        return std::make_shared<Sound>(data);
    }
}
