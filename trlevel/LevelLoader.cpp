#include "LevelLoader.h"
#include "Level.h"

using namespace trview;

namespace trlevel
{
    LevelLoader::LevelLoader(const std::shared_ptr<ILog>& log)
        : _log(log)
    {
    }

    std::unique_ptr<ILevel> LevelLoader::load_level(const std::string& filename) const
    {
        return std::make_unique<Level>(filename, _log);
    }
}
