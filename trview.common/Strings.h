#pragma once

#include <string>

namespace trview
{
    /// Convert the specified string to utf-8.
    /// @param value The UTF-16 value to convert.
    /// @returns The converted string.
    std::string to_utf8(const std::wstring& value);

    /// Convert the specified string to utf-16.
    /// @param value The utf-8 value to convert.
    /// @returns The converted string.
    std::wstring to_utf16(const std::string& value);
}
