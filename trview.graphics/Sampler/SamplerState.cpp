#include "SamplerState.h"

namespace trview
{
    namespace graphics
    {
        ISamplerState::~ISamplerState()
        {
        }

        SamplerState::SamplerState(
            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
            const Microsoft::WRL::ComPtr<ID3D11SamplerState>& sampler_state)
            : _context(context), _sampler_state(sampler_state)
        {
        }

        void SamplerState::apply()
        {
            _context->PSSetSamplers(0, 1, _sampler_state.GetAddressOf());
        }
    }
}
