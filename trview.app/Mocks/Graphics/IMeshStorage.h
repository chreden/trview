#pragma once

#include <trview.app/Graphics/IMeshStorage.h>

namespace trview
{
    class MockMeshStorage final : public IMeshStorage
    {
        MOCK_METHOD(Mesh*, mesh, (uint32_t), (const));
    };
}