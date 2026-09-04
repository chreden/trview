module;

#include <gmock/gmock.h>

export module trview.app:MockMeshStorage;

import :IMeshStorage;

namespace trview
{
    namespace mocks
    {
        struct MockMeshStorage : public IMeshStorage
        {
            MockMeshStorage();
            virtual ~MockMeshStorage();
            MOCK_METHOD(std::shared_ptr<IMesh>, mesh, (uint32_t), (const, override));
        };
    }
}