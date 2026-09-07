module;

#include <d3d11.h>
#include <wrl/client.h>

export module trview.graphics:SamplerState;

import :ISamplerState;
import :IDevice;

namespace trview
{
    namespace graphics
    {
        export class SamplerState final : public ISamplerState
        {
        public:
            SamplerState(
                const std::weak_ptr<IDevice>& device,
                const Microsoft::WRL::ComPtr<ID3D11SamplerState>& sampler_state);
            virtual ~SamplerState() = default;
            void apply() override;
            void set_filter_mode(FilterMode mode) override;
        private:
            std::weak_ptr<IDevice> _device;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;
        };
    }
}
