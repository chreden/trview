#include "Strings.h"
#include <algorithm>
#include <format>
#include <filesystem>

namespace trview
{
    std::string to_utf8(const std::wstring& value)
    {
        std::vector<char> output(WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, nullptr, 0, nullptr, nullptr), 0);
        if (!output.empty())
        {
            WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, &output[0], static_cast<int>(output.size()), nullptr, nullptr);
        }
        return &output[0];
    }

    std::wstring to_utf16(const std::string& value)
    {
        std::vector<wchar_t> output(MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0), 0);
        if (!output.empty())
        {
            MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, &output[0], static_cast<int>(output.size()));
        }
        return &output[0];
    }

    std::string format_binary(uint16_t value)
    {
        std::string result = std::bitset<5>(value).to_string();
        return std::string(result.rbegin(), result.rend());
    }

    bool is_link(const std::wstring& text)
    {
        return text.find(L"http://") == 0 || text.find(L"https://") == 0 || text.find(L"www.") == 0;
    }

    std::string to_lowercase(const std::string& value)
    {
        std::string result;
        std::transform(value.begin(), value.end(), std::back_inserter(result),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return result;
    }

    std::string filename_without_path(const std::string& filename)
    {
        return std::filesystem::path(filename).filename().string();
    }

    std::string path_for_filename(const std::string& filename)
    {
        return std::filesystem::path(filename).parent_path().string();
    }
}
