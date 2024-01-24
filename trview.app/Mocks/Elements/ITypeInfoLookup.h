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
            MOCK_METHOD(std::string, lookup_type_name, (trlevel::LevelVersion, uint32_t, uint16_t), (const, override));
            MOCK_METHOD(bool, is_pickup, (trlevel::LevelVersion, uint32_t), (const, override));
            MOCK_METHOD(std::optional<TypeInfo>, lookup, (trlevel::LevelVersion, uint32_t, int16_t), (const, override));
        };
    }
}
