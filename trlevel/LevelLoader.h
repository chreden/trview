#pragma once

#include <trview.common/Logs/ILog.h>

#include "ILevelLoader.h"

namespace trlevel
{
    class LevelLoader final : public ILevelLoader
    {
    public:
        explicit LevelLoader(const std::shared_ptr<trview::ILog>& log);
        virtual ~LevelLoader() = default;
        virtual std::unique_ptr<ILevel> load_level(const std::string& filename) const;
    private:
        std::shared_ptr<trview::ILog> _log;
    };
}