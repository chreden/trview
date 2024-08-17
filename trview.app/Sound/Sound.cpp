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
        bool initialised{ false };
        ma_decoder decoder;
        ma_device_config deviceConfig;
        ma_device device;
    };

    Sound::Sound(const std::vector<uint8_t>& data)
        : _data(data), _impl(std::make_unique<Impl>())
    {
        // if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        //     printf("Failed to open playback device.\n");
        //     ma_decoder_uninit(&decoder);
        //     // return -3;
        // }

        // if (ma_device_start(&device) != MA_SUCCESS) {
        //     printf("Failed to start playback device.\n");
        //     ma_device_uninit(&device);
        //     ma_decoder_uninit(&decoder);
        //     // return -4;
        // }
    }

    Sound::~Sound()
    {
        ma_device_uninit(&_impl->device);
        ma_decoder_uninit(&_impl->decoder);
    }

    void Sound::play()
    {
        initialise();
        ma_decoder_seek_to_pcm_frame(&_impl->decoder, 0);
        ma_device_start(&_impl->device);
    }

    void Sound::initialise()
    {
        if (!_impl->initialised)
        {
            ma_decoder_init_memory(&_data[0], _data.size(), nullptr, &_impl->decoder);
            _impl->deviceConfig = ma_device_config_init(ma_device_type_playback);
            _impl->deviceConfig.playback.format = _impl->decoder.outputFormat;
            _impl->deviceConfig.playback.channels = _impl->decoder.outputChannels;
            _impl->deviceConfig.sampleRate = _impl->decoder.outputSampleRate;
            _impl->deviceConfig.dataCallback = data_callback;
            _impl->deviceConfig.pUserData = &_impl->decoder;
            ma_device_init(NULL, &_impl->deviceConfig, &_impl->device);
            _impl->initialised = true;
        }
    }

    std::shared_ptr<ISound> create_sound(const std::vector<uint8_t>& data)
    {
        return std::make_shared<Sound>(data);
    }
}
