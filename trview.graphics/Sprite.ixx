module;

#include <wrl/client.h>
#include <d3d11.h>
#include <SimpleMath.h>

export module trview.graphics:Sprite;

import std;
import std.compat;
import trview.common;
import :ISprite;
import :IDevice;
import :IShaderStorage;
import :Texture;
import :IShader;

namespace trview
{
    namespace graphics
    {
        export class Sprite : public ISprite
        {
        public:
            Sprite(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<IShaderStorage>& shader_storage, const Size& host_size);

            virtual void render(const Texture& texture, float x, float y, float width, float height, DirectX::SimpleMath::Color colour = { 1,1,1,1 }) override;

            virtual Size host_size() const override;

            virtual void set_host_size(const Size& size) override;

            Sprite(const Sprite&) = delete;
            Sprite& operator=(const Sprite&) = delete;
        private:
            void create_matrix();

            void update_matrix(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, float x, float y, float width, float height, const DirectX::SimpleMath::Color& colour);

            std::shared_ptr<graphics::IDevice> _device;
            Microsoft::WRL::ComPtr<ID3D11Buffer>       _vertex_buffer;
            Microsoft::WRL::ComPtr<ID3D11Buffer>       _index_buffer;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;
            Microsoft::WRL::ComPtr<ID3D11Buffer>       _matrix_buffer;
            graphics::IShader*          _vertex_shader;
            graphics::IShader*          _pixel_shader;
            Size                        _host_size;
        };
    }
}
