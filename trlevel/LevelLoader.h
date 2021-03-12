#pragma once

#include "ILevelLoader.h"

namespace trlevel
{
    class LevelLoader final : public ILevelLoader
    {
    public:
        virtual ~LevelLoader() = default;
        virtual std::unique_ptr<ILevel> load_level(const std::string& filename) const;
    };
}