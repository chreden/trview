#pragma once

#include "IFiles.h"

namespace trview
{
    class Files final : public IFiles
    {
    public:
        virtual ~Files() = default;
        virtual std::optional<std::vector<uint8_t>> load_file(const std::string& filename) const override;
        virtual void save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const override;
        virtual void save_file(const std::string& filename, const std::string& text) const override;
    };
}
