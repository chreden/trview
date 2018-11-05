#include "Strings.h"
#include <vector>
#include <Windows.h>

namespace trview
{
    std::string to_utf8(const std::wstring& value)
    {
        std::vector<char> output(WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, nullptr, 0, nullptr, nullptr), 0);
        if (!output.empty())
        {
            WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, &output[0], output.size(), nullptr, nullptr);
        }
        return &output[0];
    }

    std::wstring to_utf16(const std::string& value)
    {
        std::vector<wchar_t> output(MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0), 0);
        if (!output.empty())
        {
            MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, &output[0], output.size());
        }
        return &output[0];
    }
}
