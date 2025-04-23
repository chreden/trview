#pragma once

#include <trview.common/IFiles.h>
#include "ILevelNameLookup.h"

namespace trview
{
    class LevelNameLookup final : public ILevelNameLookup
    {
    public:
        explicit LevelNameLookup(const std::shared_ptr<IFiles>& files, const std::string& level_hashes_json);
        virtual ~LevelNameLookup() = default;
        std::string lookup(const std::weak_ptr<ILevel>& level) const override;
    private:
        std::shared_ptr<IFiles> _files;
        std::unordered_map<std::string, std::string> _hashes;
    };
}
