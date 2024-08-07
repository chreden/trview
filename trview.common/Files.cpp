#include "Files.h"
#include "Strings.h"
#include <shlobj.h>

namespace trview
{
    namespace
    {
        struct SafePath
        {
            wchar_t* path;
            ~SafePath()
            {
                if (path)
                {
                    CoTaskMemFree(path);
                }
            }
        };
    }

    IFiles::~IFiles()
    {
    }

    std::string Files::appdata_directory() const
    {
        SafePath path;
        if (S_OK != SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path.path))
        {
            return std::string();
        }
        return to_utf8(path.path);
    }

    std::string Files::fonts_directory() const
    {
        SafePath path;
        if (S_OK != SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &path.path))
        {
            return std::string();
        }
        return to_utf8(path.path);
    }

    void Files::delete_file(const std::string& filename) const
    {
        DeleteFile(to_utf16(filename).c_str());
    }

    bool Files::create_directory(const std::string& directory) const
    {
        return CreateDirectory(to_utf16(directory).c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    std::optional<std::vector<uint8_t>> Files::load_file(const std::string& filename) const
    {
        return load_file(to_utf16(filename));
    }

    std::optional<std::vector<uint8_t>> Files::load_file(const std::wstring& filename) const
    {
        try
        {
            std::ifstream infile;
            infile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
            infile.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);

            if (!infile.is_open())
            {
                return std::nullopt;
            }

            const auto length = infile.tellg();
            if (!length)
            {
                return { {} };
            }

            infile.seekg(0, std::ios::beg);
            std::vector<uint8_t> bytes(static_cast<uint32_t>(length));
            infile.read(reinterpret_cast<char*>(&bytes[0]), length);
            return bytes;
        }
        catch(...)
        {
            return std::nullopt;
        }
    }

    void Files::save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const
    {
        std::ofstream outfile;
        outfile.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
        outfile.open(to_utf16(filename), std::ios::out | std::ios::binary);
        outfile.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
    }

    void Files::save_file(const std::string& filename, const std::string& text) const
    {
        std::ofstream outfile;
        outfile.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
        outfile.open(to_utf16(filename), std::ios::out);
        outfile << text;
    }

    std::vector<IFiles::File> Files::get_files(const std::string& folder, const std::string& pattern) const
    {
        std::vector<std::wstring> patterns;
        std::size_t index = 0;
        do
        {
            std::size_t next = pattern.find(',', index);
            patterns.push_back(to_utf16(pattern.substr(index, next - index)));
            index = ++next;
        } while (index);

        return get_files(to_utf16(folder), patterns);
    }

    std::vector<IFiles::Directory> Files::get_directories(const std::string& folder) const
    {
        std::vector<Directory> data;
        
        std::wstring folder_w = to_utf16(folder);

        WIN32_FIND_DATA fd;
        HANDLE find = FindFirstFile((folder_w + L"\\*").c_str(), &fd);

        if (find != INVALID_HANDLE_VALUE)
        {
            do
            {
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                    std::wstring(fd.cFileName) != L"." &&
                    std::wstring(fd.cFileName) != L"..")
                {
                    Directory directory{ to_utf8(folder_w + L"\\" + fd.cFileName), to_utf8(fd.cFileName) };
                    data.push_back(directory);
                }
            } while (FindNextFile(find, &fd) != 0);
        }

        return data;
    }

    std::string Files::working_directory() const
    {
        DWORD length = GetCurrentDirectory(0, nullptr);
        std::vector<wchar_t> buffer (static_cast<std::size_t>(length) + 1, static_cast<wchar_t>(0));
        GetCurrentDirectory(static_cast<DWORD>(buffer.size()), &buffer[0]);
        return to_utf8(&buffer[0]);
    }

    void Files::set_working_directory(const std::string& directory)
    {
        SetCurrentDirectory(to_utf16(directory).c_str());
    }

    std::vector<IFiles::File> Files::get_files(const std::wstring& folder, const std::vector<std::wstring>& patterns) const
    {
        std::vector<File> data;

        for (const auto& pattern : patterns)
        {
            WIN32_FIND_DATA fd;
            HANDLE find = FindFirstFile((folder + pattern).c_str(), &fd);

            if (find == INVALID_HANDLE_VALUE)
            {
                continue;
            }

            do
            {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    File file{ to_utf8(folder + L"\\" + fd.cFileName), to_utf8(fd.cFileName), fd.nFileSizeLow };
                    data.push_back(file);
                }
            } while (FindNextFile(find, &fd) != 0);
        }

        return data;
    }
}
