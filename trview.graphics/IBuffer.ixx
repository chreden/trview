module;

#include <d3d11.h>

export module trview.graphics:IBuffer;

import std;

namespace trview
{
    namespace graphics
    {
        export struct IBuffer
        {
            enum class ApplyTo
            {
                VS,
                PS
            };

            using ConstantSource = std::function<std::unique_ptr<IBuffer>(uint32_t)>;

            virtual ~IBuffer() = 0;
            virtual void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, ApplyTo target) = 0;
            virtual void set_data(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const void* const data, uint32_t size) = 0;
        };

        export template <typename T>
        void set_data(IBuffer& buffer, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const T& value);
    }
}

#include "IBuffer.inl"
