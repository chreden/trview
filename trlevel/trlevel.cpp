#include "trlevel.h"
#include "Level.h"

namespace trlevel
{
    std::unique_ptr<ILevel> load_level(const std::wstring& filename)
    {
        return std::make_unique<Level>(filename);
    }
}