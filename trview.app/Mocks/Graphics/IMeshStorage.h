#pragma once

#include "../../Graphics/IMeshStorage.h"

namespace trview
{
    namespace mocks
    {
        struct MockMeshStorage : public IMeshStorage
        {
            virtual ~MockMeshStorage() = default;
            MOCK_METHOD(std::shared_ptr<IMesh>, mesh, (uint32_t), (const, override));
        };
    }
}