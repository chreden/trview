#pragma once

#include "../../Elements/ITypeNameLookup.h"

namespace trview
{
    namespace mocks
    {
        class MockTypeNameLookup : public ITypeNameLookup
        {
        public:
            MOCK_METHOD(std::wstring, lookup_type_name, (trlevel::LevelVersion, uint32_t), (const, override));
        };
    }
}