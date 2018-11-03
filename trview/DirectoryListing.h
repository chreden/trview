#pragma once

#include <vector>
#include <string>

namespace trview
{

struct File
{
    std::string path, friendly_name; 
    uint32_t size; 
};

class DirectoryListing
{
public:
    /// Create a new directory listing.
    /// @param path The directory to get the file listing from.
    explicit DirectoryListing(const std::string& path);

    /// Gets a list of files in the directory.
    /// @pattern Comma separated list of file filters.
    /// @returns An empty vector if no path has been set, otherwise 
    ///          the files in the directory.
    std::vector<File> GetFiles(const std::string& pattern="\\*.TR*,\\*.PHD") const; 
private:
    std::vector<File> GetFiles(const std::vector<std::wstring>& patterns) const;

    std::wstring _path; 
};

}