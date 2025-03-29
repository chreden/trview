#pragma once

#include <vector>
#include <functional>

namespace trlevel
{
    struct ILevel;

    struct IPack
    {
        struct Part
        {
            uint32_t start;
            uint32_t size;
            std::vector<uint8_t> data;
            std::shared_ptr<ILevel> level;
        };

        using Source = std::function<std::shared_ptr<IPack>(std::basic_ispanstream<uint8_t>&)>;
        virtual ~IPack() = 0;
        virtual void load() = 0;
        virtual const std::vector<Part>& parts() const = 0;
        virtual std::string filename() const = 0;
        virtual void set_filename(const std::string& filename) = 0;
    };

    std::string pack_filename(const std::string& filename);
}

