#pragma once

#include "../../Elements/ITypeNameLookup.h"

namespace trview
{
    namespace mocks
    {
        struct MockTypeNameLookup : public ITypeNameLookup
        {
            MockTypeNameLookup();
            virtual ~MockTypeNameLookup();
            MOCK_METHOD(std::string, lookup_type_name, (trlevel::LevelVersion, uint32_t, uint16_t), (const, override));
            MOCK_METHOD(bool, is_pickup, (trlevel::LevelVersion, uint32_t), (const, override));
            MOCK_METHOD(std::optional<TypeInfo>, lookup, (trlevel::LevelVersion, uint32_t, int16_t), (const, override));
        };
    }
}
