#include "Buffer.h"
#include "IDevice.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        IBuffer::~IBuffer()
        {
        }

        Buffer::Buffer(const std::shared_ptr<IDevice>& device, uint32_t size)
        {
            D3D11_BUFFER_DESC buffer_desc;
            memset(&buffer_desc, 0, sizeof(buffer_desc));
            buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            buffer_desc.ByteWidth = size;
            buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
            buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            _buffer = device->create_buffer(buffer_desc, std::nullopt);
        }

        void Buffer::apply(const ComPtr<ID3D11DeviceContext>& context, ApplyTo target)
        {
            switch (target)
            {
            case ApplyTo::VS:
                context->VSSetConstantBuffers(0, 1, _buffer.GetAddressOf());
                break;
            case ApplyTo::PS:
                context->PSSetConstantBuffers(0, 1, _buffer.GetAddressOf());
                break;
            }
        }

        void Buffer::set_data(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const void* const data, uint32_t size)
        {
            D3D11_MAPPED_SUBRESOURCE resource;
            memset(&resource, 0, sizeof(resource));
            context->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
            memcpy(resource.pData, data, size);
            context->Unmap(_buffer.Get(), 0);
        }
    }
}
