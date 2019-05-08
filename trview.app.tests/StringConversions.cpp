#include "StringConversions.h"

using namespace DirectX::SimpleMath;
using namespace trview;

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

            std::wstring ToString(CameraMode mode)
            {
                switch (mode)
                {
                case CameraMode::Free:
                    return L"Free";
                case CameraMode::Orbit:
                    return L"Orbit";
                case CameraMode::Axis:
                    return L"Axis";
                }
                return L"Unknown";
            }
        }
    }
}
