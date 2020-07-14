#include "trlevel.h"
#include "Level.h"

namespace trlevel
{
    std::unique_ptr<ILevel> load_level(const std::string& filename)
    {
        return std::make_unique<Level>(filename);
    }
}