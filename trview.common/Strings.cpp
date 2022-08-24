#include "Strings.h"
#include <algorithm>
#include <format>

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
        return std::bitset<5>(value).to_string();
    }

    bool is_link(const std::wstring& text)
    {
        return text.find(L"http://") == 0 || text.find(L"https://") == 0 || text.find(L"www.") == 0;
    }

    std::string to_lowercase(const std::string& value)
    {
        std::string result;
        std::transform(value.begin(), value.end(), std::back_inserter(result), ::tolower);
        return result;
    }
}
