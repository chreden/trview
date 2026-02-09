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
        std::optional<Name> lookup(const std::weak_ptr<ILevel>& level) const override;
        std::optional<Name> lookup(const std::weak_ptr<trlevel::ILevel>& level) const override;
        std::vector<int32_t> bonus_items(const std::weak_ptr<ILevel>& level) const override;
    private:
        std::optional<Name> get_name(const std::string& filename, trlevel::PlatformAndVersion platform_and_version, const std::string& hash) const;
        std::optional<Name> check_remastered(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const;
        std::optional<Name> check_trx(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const;
        bool is_trx() const;

        std::vector<int32_t> get_bonus_items(const std::string& filename, trlevel::PlatformAndVersion platform_and_version, const std::string& hash) const;
        std::optional<std::vector<int32_t>> check_remastered_bonus_items(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const;


        std::shared_ptr<IFiles> _files;
        std::unordered_map<std::string, std::string> _hashes;
    };
}
