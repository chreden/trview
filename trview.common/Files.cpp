#include "Files.h"

namespace trview
{
    void Files::save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const
    {
        std::ofstream outfile;
        outfile.open(filename, std::ios::out | std::ios::binary);
        outfile.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
    }
}
