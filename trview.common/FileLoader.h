/// @file FileLoader.h
/// @brief Loads files and returns the bytes read from the file.
/// 
/// Helper function to read the bytes in a file in a single call.

#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace trview
{
    /// Read the contents of a file.
    /// @param filename The file to read.
    /// @returns The bytes that were read from the file.
    std::vector<uint8_t> load_file(std::wstring filename);
}