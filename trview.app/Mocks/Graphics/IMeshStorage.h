#pragma once

#include "../../Graphics/IMeshStorage.h"

namespace trview
{
    namespace mocks
    {
        struct MockMeshStorage : public IMeshStorage
        {
            MockMeshStorage();
            virtual ~MockMeshStorage();
            MOCK_METHOD(std::shared_ptr<IMesh>, mesh, (uint32_t), (const, override));
            MOCK_METHOD((std::map<uint32_t, std::weak_ptr<IMesh>>), meshes, (), (const, override));
        };
    }
}