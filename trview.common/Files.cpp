#include "Files.h"
#include "Strings.h"

namespace trview
{
    std::vector<uint8_t> Files::load_file(const std::string& filename) const
    {
        std::ifstream infile;
        infile.open(to_utf16(filename), std::ios::in | std::ios::binary | std::ios::ate);

        const auto length = infile.tellg();
        if (!length)
        {
            return {};
        }

        infile.seekg(0, std::ios::beg);
        std::vector<uint8_t> bytes(static_cast<uint32_t>(length));
        infile.read(reinterpret_cast<char*>(&bytes[0]), length);
        return bytes;
    }

    void Files::save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const
    {
        std::ofstream outfile;
        outfile.open(to_utf16(filename), std::ios::out | std::ios::binary);
        outfile.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
    }

    void Files::save_file(const std::string& filename, const std::string& text) const
    {
        std::ofstream outfile;
        outfile.open(to_utf16(filename), std::ios::out);
        outfile << text;
    }
}
