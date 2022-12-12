#pragma once

#include "IBuffer.h"

namespace trview
{
    namespace graphics
    {
        struct IDevice;
        class Buffer final : public IBuffer
        {
        public:
            virtual ~Buffer() = default;
            Buffer(const std::shared_ptr<IDevice>& device, uint32_t size);
            virtual void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, ApplyTo target) override;
            virtual void set_data(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const void* const data, uint32_t size) override;
        private:
            Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
        };
    }
}
