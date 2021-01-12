#include "Strings.h"

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

    std::wstring format_bool(bool value)
    {
        std::wstringstream stream;
        stream << std::boolalpha << value;
        return stream.str();
    }

    std::wstring format_binary(uint16_t value)
    {
        std::wstringstream stream;
        stream << std::bitset<5>(value);
        const auto result = stream.str();
        return std::wstring(result.rbegin(), result.rend());
    }

    bool is_link(const std::wstring& text)
    {
        return text.find(L"http://") == 0 || text.find(L"https://") == 0 || text.find(L"www.") == 0;
    }
}
