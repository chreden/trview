#pragma once

#include "../IFiles.h"

namespace trview
{
    namespace mocks
    {
        struct MockFiles : public IFiles
        {
            MockFiles();
            virtual ~MockFiles();
            MOCK_METHOD(std::string, appdata_directory, (), (const, override));
            MOCK_METHOD(std::string, fonts_directory, (), (const, override));
            MOCK_METHOD(bool, create_directory, (const std::string&), (const, override));
            MOCK_METHOD(void, delete_file, (const std::string&), (const, override));
            MOCK_METHOD(std::optional<std::vector<uint8_t>>, load_file, (const std::string&), (const, override));
            MOCK_METHOD(std::optional<std::vector<uint8_t>>, load_file, (const std::wstring&), (const, override));
            MOCK_METHOD(void, save_file, (const std::string&, const std::vector<uint8_t>&), (const, override));
            MOCK_METHOD(void, save_file, (const std::string&, const std::string&), (const, override));
            MOCK_METHOD(std::vector<File>, get_files, (const std::string&, const std::string&), (const, override));
            MOCK_METHOD(std::vector<Directory>, get_directories, (const std::string&), (const, override));
            MOCK_METHOD(std::string, working_directory, (), (const, override));
            MOCK_METHOD(void, set_working_directory, (const std::string&), (override));
        };
    }
}
