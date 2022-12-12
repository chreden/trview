#pragma once

namespace trview
{
    namespace graphics
    {
        template <typename T>
        void set_data(IBuffer& buffer, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const T& value)
        {
            buffer.set_data(context, &value, sizeof(value));
        }
    }
}
