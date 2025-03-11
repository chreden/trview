#pragma once

#include "../../Elements/ITypeInfoLookup.h"

namespace trview
{
    namespace mocks
    {
        struct MockTypeInfoLookup : public ITypeInfoLookup
        {
            MockTypeInfoLookup();
            virtual ~MockTypeInfoLookup();
            MOCK_METHOD(TypeInfo, lookup, (trlevel::PlatformAndVersion, uint32_t, int16_t), (const, override));
        };
    }
}
