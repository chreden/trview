#pragma once

#include "../../Elements/Remastered/INgPlusSwitcher.h"

namespace trview
{
    namespace mocks
    {
        struct MockNgPlusSwitcher : public INgPlusSwitcher
        {
            MockNgPlusSwitcher();
            ~MockNgPlusSwitcher();
            MOCK_METHOD((std::unordered_map<uint16_t, std::shared_ptr<IItem>>), create_for_level, (const std::shared_ptr<ILevel>&, const trlevel::ILevel&, const IModelStorage&), (const, override));
        };
    }
}
