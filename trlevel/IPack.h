#pragma once

#include "LevelVersion.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include <trview.common/IFiles.h>

namespace trlevel
{
    struct IPack
    {
        struct Part
        {
            uint32_t start;
            uint32_t size;
            std::vector<uint8_t> data;
            std::optional<trlevel::PlatformAndVersion> version;
        };

        using Source = std::function<std::shared_ptr<IPack>(std::basic_ispanstream<uint8_t>&)>;
        virtual ~IPack() = 0;
        virtual void load() = 0;
        virtual const std::vector<Part>& parts() const = 0;
        virtual std::string filename() const = 0;
        virtual void set_filename(const std::string& filename) = 0;
    };

    std::string pack_filename(const std::string& filename);
    std::optional<std::vector<uint8_t>> pack_entry(const IPack& pack, uint32_t offset);
    std::vector<trview::IFiles::File> valid_pack_levels(const IPack& pack);
}

