#include "DirectoryListing.h"
#include <trview.common/Strings.h>

namespace trview
{

namespace
{
    /// Checks if a path is a directory.
    /// @returns True if the given path is a directory, false otherwise.
    bool
    IsDirectory(const std::wstring& path)
    {
        DWORD attr = GetFileAttributes(path.c_str());
        return (bool)(attr & FILE_ATTRIBUTE_DIRECTORY);
    }
}

DirectoryListing::DirectoryListing(const std::string& path)
    : _path(to_utf16(path))
{
    if (!IsDirectory(_path))
    {
        throw std::invalid_argument("'path' argument must be a directory");
    }
}

std::vector<File> 
DirectoryListing::GetFiles(const std::string& pattern) const
{
    std::vector<std::wstring> patterns;
    std::size_t index = 0;
    do
    {
        std::size_t next = pattern.find(',', index);
        patterns.push_back(to_utf16(pattern.substr(index, next - index)));
        index = ++next;
    } while(index);

    return GetFiles(patterns);
}

std::vector<File>
DirectoryListing::GetFiles(const std::vector<std::wstring>& patterns) const
{
    std::vector<File> data;

    for (const auto& pattern : patterns)
    {
        WIN32_FIND_DATA fd;
        HANDLE find = FindFirstFile((_path + pattern).c_str(), &fd);

        if (find == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            if (error != ERROR_FILE_NOT_FOUND)
            {
                throw std::runtime_error("could not open directory listing");
            }
            continue;
        }

        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                File file{ to_utf8(_path + L"\\" + fd.cFileName), to_utf8(fd.cFileName), fd.nFileSizeLow };
                data.push_back(file);
            }
        } while
            (FindNextFile(find, &fd) != 0);
    }

    return data;
}

}
