#include "Sprite.h"

#include <SimpleMath.h>

#include <vector>

#include <trview.common/FileLoader.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            namespace
            {
                struct Vertex
                {
                    DirectX::SimpleMath::Vector3 pos;
                    DirectX::SimpleMath::Vector2 uv;
                };
            }

            Sprite::Sprite(const CComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, uint32_t width, uint32_t height)
                : _device(device), _host_width(width), _host_height(height)
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

                HRESULT hr = device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);

                uint32_t indices[] = { 0, 1, 2, 3 };

                D3D11_BUFFER_DESC index_desc;
                memset(&index_desc, 0, sizeof(index_desc));
                index_desc.Usage = D3D11_USAGE_DEFAULT;
                index_desc.ByteWidth = sizeof(uint32_t) * 4;
                index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA index_data;
                memset(&index_data, 0, sizeof(index_data));
                index_data.pSysMem = indices;

                hr = device->CreateBuffer(&index_desc, &index_data, &_index_buffer);

                _vertex_shader = shader_storage.shader("ui_vertex_shader");
                _pixel_shader = shader_storage.shader("ui_pixel_shader");

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
                device->CreateSamplerState(&desc, &_sampler_state);

                create_matrix();
            }

            void Sprite::set_host_size(uint32_t width, uint32_t height)
            {
                _host_width = width;
                _host_height = height;
            }

            void Sprite::render(CComPtr<ID3D11DeviceContext> context, CComPtr<ID3D11ShaderResourceView> texture, float x, float y, float width, float height, DirectX::SimpleMath::Color colour)
            {
                update_matrix(context, x, y, width, height, colour);

                context->PSSetShaderResources(0, 1, &texture.p);
                context->PSSetSamplers(0, 1, &_sampler_state.p);

                // select which vertex buffer to display
                UINT stride = sizeof(Vertex);
                UINT offset = 0;

                _vertex_shader->apply(context);
                _pixel_shader->apply(context);

                context->IASetVertexBuffers(0, 1, &_vertex_buffer.p, &stride, &offset);
                context->IASetIndexBuffer(_index_buffer, DXGI_FORMAT_R32_UINT, 0);
                context->VSSetConstantBuffers(0, 1, &_matrix_buffer.p);
                context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                context->DrawIndexed(4, 0, 0);
            }

            void Sprite::create_matrix()
            {
                using namespace DirectX::SimpleMath;
                D3D11_BUFFER_DESC desc;
                memset(&desc, 0, sizeof(desc));

                desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                desc.ByteWidth = sizeof(Matrix) + sizeof(Color);
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

                _device->CreateBuffer(&desc, nullptr, &_matrix_buffer);
            }

            void Sprite::update_matrix(CComPtr<ID3D11DeviceContext> context, float x, float y, float width, float height, const DirectX::SimpleMath::Color& colour)
            {
                using namespace DirectX::SimpleMath;

                D3D11_MAPPED_SUBRESOURCE mapped_resource;
                memset(&mapped_resource, 0, sizeof(mapped_resource));

                struct Data
                {
                    Matrix matrix;
                    Color colour;
                };

                // Need to scale the quad so that it is a certain size. Will need to know the 
                // size of the host window as well as the size that we want the texture window
                // to be. Then create a scaling matrix and throw it in to the shader.
                auto scaling = Matrix::CreateScale(width / _host_width, height / _host_height, 1);

                // Try to make the appropriate translation matrix to move it to the top left of the screen.
                auto translation = Matrix::CreateTranslation(-1.f + width / _host_width + (x * 2) / _host_width, 1.f - height / _host_height - (y * 2) / _host_height, 0);

                Data data{ scaling * translation, colour };

                context->Map(_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
                memcpy(mapped_resource.pData, &data, sizeof(data));
                context->Unmap(_matrix_buffer, 0);
            }
        }
    }
}
