export module trview.graphics:PixelShader;

import std;
import :IShader;
import :IDevice;

namespace trview
{
    namespace graphics
    {
        export class PixelShader final : public IShader
        {
        public:
            PixelShader(const graphics::IDevice& device, const std::vector<uint8_t>& data);

            virtual ~PixelShader() = default;

            virtual void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override;
        private:
            Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixel_shader;
        };
    }
}

