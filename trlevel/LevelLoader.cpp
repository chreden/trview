#include "LevelLoader.h"
#include "Level.h"

namespace trlevel
{
    std::unique_ptr<ILevel> LevelLoader::load_level(const std::string& filename) const
    {
        return std::make_unique<Level>(filename);
    }
}
