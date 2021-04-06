#include "Sprite.h"
#include <trview.common/FileLoader.h>
#include "IShaderStorage.h"
#include "IShader.h"
#include "Texture.h"
#include <trview.app/Geometry/IMesh.h>

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        namespace
        {
            struct Vertex
            {
                DirectX::SimpleMath::Vector3 pos;
                DirectX::SimpleMath::Vector2 uv;
            };
        }

        Sprite::Sprite(const std::shared_ptr<IDevice>& device, const std::shared_ptr<IShaderStorage>& shader_storage, const Size& host_size)
            : _device(device), _host_size(host_size)
        {
            using namespace DirectX::SimpleMath;

            Vertex vertices[] =
            {
                { Vector3(-1.0f, 1.0f, 0.0f), Vector2::Zero },
                { Vector3(1.0f, 1.0f, 0.0f), Vector2(1,0) },
                { Vector3(-1.0f, -1.0f, 0.0f), Vector2(0,1) },
                { Vector3(1.0f, -1.0f, 0.0f), Vector2(1,1) }
            };

            D3D11_BUFFER_DESC vertex_desc;
            memset(&vertex_desc, 0, sizeof(vertex_desc));
            vertex_desc.Usage = D3D11_USAGE_DEFAULT;
            vertex_desc.ByteWidth = sizeof(Vertex) * 4;
            vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vertex_data;
            memset(&vertex_data, 0, sizeof(vertex_data));
            vertex_data.pSysMem = vertices;

            _vertex_buffer = _device->create_buffer(vertex_desc, vertex_data);

            uint32_t indices[] = { 0, 1, 2, 3 };

            D3D11_BUFFER_DESC index_desc;
            memset(&index_desc, 0, sizeof(index_desc));
            index_desc.Usage = D3D11_USAGE_DEFAULT;
            index_desc.ByteWidth = sizeof(uint32_t) * 4;
            index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

            D3D11_SUBRESOURCE_DATA index_data;
            memset(&index_data, 0, sizeof(index_data));
            index_data.pSysMem = indices;

            _index_buffer = _device->create_buffer(index_desc, index_data);

            _vertex_shader = shader_storage->get("ui_vertex_shader");
            _pixel_shader = shader_storage->get("ui_pixel_shader");

            // Create a texture sampler state description.
            D3D11_SAMPLER_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.MaxAnisotropy = 1;
            desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
            desc.MaxLOD = D3D11_FLOAT32_MAX;

            // Create the texture sampler state.
            _sampler_state = _device->create_sampler_state(desc);

            create_matrix();
        }

        void Sprite::set_host_size(const Size& size)
        {
            _host_size = size;
        }

        void Sprite::render(const Texture& texture, float x, float y, float width, float height, DirectX::SimpleMath::Color colour)
        {
            auto context = _device->context();
            update_matrix(context, std::round(x), std::round(y), std::round(width), std::round(height), colour);

            _vertex_shader->apply(context);
            _pixel_shader->apply(context);
            context->PSSetShaderResources(0, 1, texture.view().GetAddressOf());
            context->PSSetSamplers(0, 1, _sampler_state.GetAddressOf());
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, _vertex_buffer.GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            context->VSSetConstantBuffers(0, 1, _matrix_buffer.GetAddressOf());
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            context->DrawIndexed(4, 0, 0);
        }

        void Sprite::create_matrix()
        {
            using namespace DirectX::SimpleMath;
            D3D11_BUFFER_DESC desc;
            memset(&desc, 0, sizeof(desc));

            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.ByteWidth = sizeof(MeshData);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            _matrix_buffer = _device->create_buffer(desc, std::optional<D3D11_SUBRESOURCE_DATA>());
        }

        void Sprite::update_matrix(const ComPtr<ID3D11DeviceContext>& context, float x, float y, float width, float height, const DirectX::SimpleMath::Color& colour)
        {
            using namespace DirectX::SimpleMath;

            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            memset(&mapped_resource, 0, sizeof(mapped_resource));

            // Need to scale the quad so that it is a certain size. Will need to know the 
            // size of the host window as well as the size that we want the texture window
            // to be. Then create a scaling matrix and throw it in to the shader.
            auto scaling = Matrix::CreateScale(width / _host_size.width, height / _host_size.height, 1);

            // Try to make the appropriate translation matrix to move it to the top left of the screen.
            auto translation = Matrix::CreateTranslation(-1.f + width / _host_size.width + (x * 2) / _host_size.width, 1.f - height / _host_size.height - (y * 2) / _host_size.height, 0);

            MeshData data{ scaling * translation, colour, Vector4::Zero };

            context->Map(_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
            memcpy(mapped_resource.pData, &data, sizeof(data));
            context->Unmap(_matrix_buffer.Get(), 0);
        }

        Size Sprite::host_size() const
        {
            return _host_size;
        }
    }
}
