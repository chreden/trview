#pragma once

#include <trview.common/IFiles.h>
#include "ILevelNameLookup.h"
#include <trlevel/LevelVersion.h>

namespace trview
{
    class LevelNameLookup final : public ILevelNameLookup
    {
    public:
        explicit LevelNameLookup(const std::shared_ptr<IFiles>& files, const std::string& level_hashes_json);
        virtual ~LevelNameLookup() = default;
        std::optional<std::string> lookup(const std::weak_ptr<ILevel>& level) const override;
        std::optional<std::string> lookup(const std::weak_ptr<trlevel::ILevel>& level) const override;
    private:
        std::optional<std::string> get_name(const std::string& filename, trlevel::PlatformAndVersion platform_and_version, const std::string& hash) const;

        std::optional<std::string> check_remastered(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const;
        std::optional<std::string> check_trx(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const;
        bool is_trx() const;


        std::shared_ptr<IFiles> _files;
        std::unordered_map<std::string, std::string> _hashes;
    };
}
