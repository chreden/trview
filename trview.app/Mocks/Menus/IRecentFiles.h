#pragma once

#include "../../Menus/IRecentFiles.h"

namespace trview
{
    namespace mocks
    {
        class MockRecentFiles : public IRecentFiles
        {
        public:
            virtual ~MockRecentFiles() = default;
            MOCK_METHOD(void, set_recent_files, (const std::list<std::string>&));
        };
    }
}
