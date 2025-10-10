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
        virtual std::optional<std::vector<uint8_t>> load_file(const std::wstring& filename) const override;
        virtual void save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const override;
        virtual void save_file(const std::string& filename, const std::string& text) const override;
        virtual std::vector<File> get_files(const std::string& folder, const std::string& pattern) const override;
        std::vector<Directory> get_directories(const std::string& folder) const override;
        std::vector<Directory> get_directories(const std::string& folder, const std::string& pattern) const override;
        std::string working_directory() const override;
        void set_working_directory(const std::string& directory) override;
    private:
        std::vector<File> get_files(const std::wstring& folder, const std::vector<std::wstring>& patterns) const;
    };
}
