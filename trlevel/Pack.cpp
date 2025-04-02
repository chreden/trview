#include "Pack.h"
#include "Level_common.h"

#include <trview.common/Strings.h>
#include <ranges>
#include <utility>

namespace trlevel
{
    namespace
    {
        struct Header
        {
            uint32_t start;
            uint32_t size;
        };
    }

    IPack::~IPack()
    {
    }

    Pack::Pack(std::basic_ispanstream<uint8_t>& file, const trlevel::ILevel::PackSource& level_source)
        : _level_source(level_source)
    {
        file.seekg(8, std::ios::beg);
        _parts = read_vector<Header>(file, 50) |
            std::views::filter([](auto&& h) { return h.size > 0; }) |
            std::views::transform([&](auto&& h) -> Part
                {
                    file.seekg(h.start, std::ios::beg);
                    std::vector<uint8_t> data;
                    if (h.size > 0)
                    {
                        data.resize(h.size);
                        file.read(&data[0], h.size);
                    }
                    return { .start = h.start, .size = h.size, .data = data };
                }) | std::ranges::to<std::vector>();
    }

    void Pack::load()
    {
        for (auto& part : _parts)
        {
            try
            {
                auto level = _level_source(std::format("pack-preview:{}", part.start), shared_from_this());
                level->load({});
                part.level = level;
            }
            catch(...)
            {
                // Consume exception
            }
        }
    }

    const std::vector<IPack::Part>& Pack::parts() const
    {
        return _parts;
    }

    std::string Pack::filename() const
    {
        return _filename;
    }

    void Pack::set_filename(const std::string& filename)
    {
        _filename = filename;
    }

    std::string pack_filename(const std::string& filename)
    {
        if (filename.starts_with("pack:"))
        {
            const std::string without_pack = filename.substr(5);
            const auto last_fs = without_pack.find_last_of('\\');
            if (last_fs != without_pack.npos)
            {
                const auto maybe_entry = without_pack.substr(last_fs + 1);
                try
                {
                    std::ignore = std::stoi(maybe_entry);
                    return filename.substr(5, last_fs);
                }
                catch (const std::invalid_argument&)
                {
                }
            }
            return filename.substr(5);
        }
        return filename;
    }
}