#pragma once

#include "../../Menus/IRecentFiles.h"

namespace trview
{
    namespace mocks
    {
        struct MockRecentFiles : public IRecentFiles
        {
            virtual ~MockRecentFiles() = default;
            MOCK_METHOD(void, set_recent_files, (const std::list<std::string>&), (override));
        };
    }
}
