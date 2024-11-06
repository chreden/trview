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
            MOCK_METHOD(SwapSet, create_for_level, (const std::shared_ptr<ILevel>&, const trlevel::ILevel&, const IMeshStorage& mesh_storage), (const, override));
        };
    }
}
