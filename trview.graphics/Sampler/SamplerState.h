#pragma once

#include "ISamplerState.h"

#include <d3d11.h>
#include <wrl/client.h>

namespace trview
{
    namespace graphics
    {
        class SamplerState final : public ISamplerState
        {
        public:
            SamplerState(
                const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
                const Microsoft::WRL::ComPtr<ID3D11SamplerState>& sampler_state);
            virtual ~SamplerState() = default;
            void apply() override;
        private:
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;
        };
    }
}
