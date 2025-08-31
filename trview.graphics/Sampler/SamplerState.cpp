#include "SamplerState.h"
#include "IDevice.h"

namespace trview
{
    namespace graphics
    {
        ISamplerState::~ISamplerState()
        {
        }

        SamplerState::SamplerState(
            const std::weak_ptr<IDevice>& device,
            const Microsoft::WRL::ComPtr<ID3D11SamplerState>& sampler_state)
            : _device(device), _sampler_state(sampler_state)
        {
        }

        void SamplerState::apply()
        {
            if (auto device = _device.lock())
            {
                device->context()->PSSetSamplers(0, 1, _sampler_state.GetAddressOf());
            }
        }

        void SamplerState::set_filter_mode(FilterMode mode)
        {
            D3D11_SAMPLER_DESC desc;
            _sampler_state->GetDesc(&desc);
            desc.Filter = mode == FilterMode::Point ? D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            if (auto device = _device.lock())
            {
                _sampler_state = device->create_sampler_state(desc);
            }
        }
    }
}
