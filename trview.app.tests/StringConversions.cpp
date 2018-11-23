#include "StringConversions.h"

using namespace DirectX::SimpleMath;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            std::wstring ToString(const Vector3& vector)
            {
                return std::to_wstring(vector.x) + L"," +
                       std::to_wstring(vector.y) + L"," +
                       std::to_wstring(vector.z);
            }
        }
    }
}
