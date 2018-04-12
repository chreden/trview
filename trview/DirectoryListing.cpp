#include "stdafx.h"
#include "DirectoryListing.h"

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
    std::vector<File> data;
    WIN32_FIND_DATA fd;

    if (_is_path_set)
    {
        HANDLE find = FindFirstFileW((_path + pattern).c_str(), &fd);

        if (find == INVALID_HANDLE_VALUE)
            throw std::runtime_error("could not open directory listing");

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

    return data; 
}

}
