#pragma once

#include <SimpleMath.h>
#include <string>

#include <trview.app/Camera/CameraMode.h>

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            std::wstring ToString(const DirectX::SimpleMath::Vector3& vector);
            std::wstring ToString(trview::CameraMode mode);
        }
    }
}