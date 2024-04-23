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

    /// Convert a value to a binary - only shows 5 bits.
    /// @param value The value to convert.
    /// @returns The converted string.
    std::string format_binary(uint16_t value);

    /// Check whether text is a link.
    /// @param value The value to check.
    /// @returns Whether it is a link.
    bool is_link(const std::wstring& text);

    /// <summary>
    /// Convert a string to lowercase.
    /// </summary>
    /// <param name="value">The string to convert.</param>
    /// <returns>The converted string.</returns>
    std::string to_lowercase(const std::string& value);

    /// <summary>
    /// Converts to a string (or does nothing if it is already a string).
    /// </summary>
    template <typename T>
    constexpr std::string get_string(T&& value);

    std::string join(std::ranges::input_range auto&& range);

    std::string filename_without_path(const std::string& filename);
}

#include "Strings.inl"
