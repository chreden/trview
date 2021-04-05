#pragma once

#include <trview.app/Graphics/IMeshStorage.h>

namespace trview
{
    namespace mocks
    {
        class MockMeshStorage final : public IMeshStorage
        {
            MOCK_METHOD(IMesh*, mesh, (uint32_t), (const));
        };
    }
}