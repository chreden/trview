#pragma once

#include <vector>
#include <string>

namespace trview
{

struct File
{
    std::wstring path, friendly_name; 
    int size; 
};

class DirectoryListing
{
public:
    DirectoryListing() = default;

    // Gets a list of files in the directory.
    // pattern: Comma separated list of file filters.
    // Returns an empty vector if no path has been set.
    std::vector<File> GetFiles(const std::wstring& pattern=L"\\*.TR*,\\*.PHD") const; 

    // Sets the directory to get the file listing from.
    void SetDirectory(const std::wstring& path);

private:
    std::vector<File> GetFiles(const std::vector<std::wstring>& patterns) const;

    // Returns true if the given path is a directory, false otherwise.
    bool IsDirectory(const std::wstring& path) const;

    std::wstring _path; 
    bool _is_path_set = false;
};

}