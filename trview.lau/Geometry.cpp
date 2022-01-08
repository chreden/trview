#include "Geometry.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lau
    {
        Vector2 convert_uv(int32_t uv)
        {
            Vector2 result;
            *(reinterpret_cast<uint64_t*>(&result.x)) =
                static_cast<uint64_t>(uv >> 0x10) << 0x30 | static_cast<uint64_t>(uv << 0x10);
            return result;
        }

        int32_t make_uv(const DirectX::SimpleMath::Vector2& input_uv)
        {
            uint64_t x = *reinterpret_cast<const uint64_t*>(&input_uv.x);
            int32_t uv = 0;
            uv |= (static_cast<int32_t>(x >> 0x30)) << 0x10;
            uv |= (static_cast<int32_t>(x >> 0x10));
            return uv;
        }
    }
}