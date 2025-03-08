#pragma once

#include <memory>
#include <spanstream>

#include "IPack.h"
#include "ILevel.h"

namespace trlevel
{
    class Pack final : public IPack, public std::enable_shared_from_this<IPack>
    {
    public:
        explicit Pack(std::basic_ispanstream<uint8_t>& file, const trlevel::ILevel::PackSource& level_source);
        virtual ~Pack() = default;
        void load() override;
        const std::vector<Part>& parts() const override;
        std::string filename() const override;
        void set_filename(const std::string& filename) override;
    private:
        std::vector<Part> _parts;
        trlevel::ILevel::PackSource _level_source;
        std::string _filename;
    };
}
