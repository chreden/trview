#include "stdafx.h"
#include "DirectoryListing.h"
#include <algorithm>

namespace trview
{

void
DirectoryListing::SetDirectory(const std::wstring& path)
{
    if (!IsDirectory(path))
        throw std::invalid_argument("'path' argument must be a directory");

    _path = path;
    _is_path_set = true;
}

bool
DirectoryListing::IsDirectory(const std::wstring& path) const
{
    DWORD attr = GetFileAttributesW(path.c_str());
    return (bool) (attr & FILE_ATTRIBUTE_DIRECTORY);
}

std::vector<File> 
DirectoryListing::GetFiles(const std::wstring& pattern) const
{
    std::vector<std::wstring> patterns;
    std::size_t index = 0;
    do
    {
        std::size_t next = pattern.find(L',', index);
        patterns.push_back(pattern.substr(index, next - index));
        index = ++next;
    } while(index);

    return GetFiles(patterns);
}

std::vector<File>
DirectoryListing::GetFiles(const std::vector<std::wstring>& patterns) const
{
    std::vector<File> data;

    if (_is_path_set)
    {
        for (const auto& pattern : patterns)
        {
            WIN32_FIND_DATA fd;
            HANDLE find = FindFirstFileW((_path + pattern).c_str(), &fd);

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
                    File file{ _path + L"\\" + fd.cFileName, fd.cFileName, fd.nFileSizeLow };
                    data.push_back(file);
                }
            } while
                (FindNextFileW(find, &fd) != 0);
        }
    }

    return data;
}

}
