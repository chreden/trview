#include "LevelLoader.h"
#include "trlevel.h"

namespace trlevel
{
    std::unique_ptr<ILevel> LevelLoader::load_level(const std::string& filename) const
    {
        return trlevel::load_level(filename);
    }
}
