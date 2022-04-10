#pragma once

#include "IFiles.h"

namespace trview
{
    class Files final : public IFiles
    {
    public:
        virtual ~Files() = default;
        virtual std::string appdata_directory() const override;
        virtual std::string fonts_directory() const override;
        virtual bool create_directory(const std::string& directory) const override;
        virtual void delete_file(const std::string& filename) const override;
        virtual std::optional<std::vector<uint8_t>> load_file(const std::string& filename) const override;
        virtual void save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const override;
        virtual void save_file(const std::string& filename, const std::string& text) const override;
        virtual std::vector<std::string> files(const std::string& directory) const override;
    };
}
