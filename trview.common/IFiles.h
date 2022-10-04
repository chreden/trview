#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <optional>

namespace trview
{
    struct IFiles
    {
        struct File
        {
            std::string path;
            std::string friendly_name;
            uint32_t size;
        };

        virtual ~IFiles() = 0;
        virtual std::string appdata_directory() const = 0;
        virtual std::string fonts_directory() const = 0;
        virtual bool create_directory(const std::string& directory) const = 0;
        virtual void delete_file(const std::string& filename) const = 0;
        virtual std::optional<std::vector<uint8_t>> load_file(const std::string& filename) const = 0;
        virtual std::optional<std::vector<uint8_t>> load_file(const std::wstring& filename) const = 0;
        virtual void save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const = 0;
        virtual void save_file(const std::string& filename, const std::string& text) const = 0;
        virtual std::vector<File> get_files(const std::string& folder, const std::string& pattern) const = 0;
    };
}
