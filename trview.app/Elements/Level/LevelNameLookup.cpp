#include "LevelNameLookup.h"
#include "../ILevel.h"

#include <filesystem>
#include <spanstream>
#include <ranges>

#include <trlevel/Level_common.h>

namespace trview
{
    namespace
    {
        std::optional<std::vector<uint8_t>> load_strings_data(const std::shared_ptr<IFiles>& files, const std::filesystem::path& level_path)
        {
            if (const auto strings_data = files->load_file(level_path.parent_path() /= "../TEXT/EN/STRINGS.TXT"))
            {
                return strings_data;
            }

            if (const auto expansion_strings_data = files->load_file(level_path.parent_path() /= "../../TEXT/EN/STRINGS.TXT"))
            {
                return expansion_strings_data;
            }

            return std::nullopt;
        }

        std::optional<std::string> read_tombpc_1_3(const std::shared_ptr<IFiles>& files, const std::string& level_filename)
        {
            const std::filesystem::path level_path{ level_filename };
            if (const auto tombpc_data = files->load_file(level_path.parent_path() /= "TOMBPC.DAT"))
            {
                try
                {
                    std::basic_ispanstream<uint8_t> file{ { *tombpc_data } };
                    file.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
                    file.seekg(326);

                    using namespace trlevel;

                    uint16_t num_levels = read<uint16_t>(file);
                    uint16_t num_chapters = read<uint16_t>(file);
                    uint16_t num_titles = read<uint16_t>(file);
                    uint16_t num_fmvs = read<uint16_t>(file);
                    uint16_t num_cutscenes = read<uint16_t>(file);
                    uint16_t num_demos = read<uint16_t>(file);
                    skip(file, 36);
                    uint16_t flags = read<uint16_t>(file);
                    skip(file, 6);
                    uint8_t xorkey = read<uint8_t>(file);
                    skip(file, 7);

                    num_demos;

                    if (!(flags & 0x100))
                    {
                        xorkey = 0;
                    }

                    auto read_string_list = [&](int count)
                        {
                            auto offsets = read_vector<uint16_t>(file, count);
                            uint16_t total_size = read<uint16_t>(file);
                            auto data = read_vector<uint8_t>(file, total_size)
                                | std::views::transform([=](auto x) -> uint8_t { return x ^ xorkey; })
                                | std::ranges::to<std::vector>();
                            return offsets
                                | std::views::transform([&](auto off) { return std::string(reinterpret_cast<char*>(&data[off])); })
                                | std::ranges::to<std::vector>();
                        };

                    const auto level_names = read_string_list(num_levels);
                    const auto chapters = read_string_list(num_chapters);
                    const auto titles = read_string_list(num_titles);
                    const auto fmvs = read_string_list(num_fmvs);
                    const auto level_paths = read_string_list(num_levels) | std::views::transform(to_lowercase) | std::ranges::to<std::vector>();
                    const auto cutscenes = read_string_list(num_cutscenes);

                    const auto lower_filename = to_lowercase(level_path.filename().string());
                    const auto found_index = std::ranges::find_if(level_paths, [&](auto& f) { return f.contains(lower_filename); });
                    if (found_index != level_paths.end())
                    {
                        return level_names[found_index - level_paths.begin()];
                    }
                }
                catch(...)
                {
                }
            }
            return std::nullopt;
        }

        std::optional<std::string> read_tombpc_4_5(const std::shared_ptr<IFiles>&, const std::string&)
        {
            return std::nullopt;
        }
    }

    ILevelNameLookup::~ILevelNameLookup()
    {
    }

    LevelNameLookup::LevelNameLookup(const std::shared_ptr<IFiles>& files)
        : _files(files)
    {
    }

    std::string LevelNameLookup::lookup(const std::weak_ptr<ILevel>& level) const
    {
        using namespace trlevel;

        const auto level_ptr = level.lock();
        if (!level_ptr)
        {
            return "";
        }

        const auto platform_and_version = level_ptr->platform_and_version();

        // Mode 1: Check in the files (TOMBPC, English.DAT, etc)
        if (platform_and_version.platform == Platform::PC)
        {
            if (platform_and_version.remastered)
            {
                const std::filesystem::path level_path{ level_ptr->filename() };
                if (const auto strings_data = load_strings_data(_files, level_path))
                {
                    std::string level_stem = level_path.stem().string();
                    std::erase(level_stem, '_');
                    const std::string level_name = std::format("LVL_{}=", level_stem);
                    const std::string strings_text = { std::from_range, strings_data.value() };
                    const auto start = strings_text.find(level_name);
                    if (start != strings_text.npos)
                    {
                        const auto end = strings_text.find('\r', start);
                        if (end != strings_text.npos)
                        {
                            return strings_text.substr(start + level_name.size(), end - start - level_name.size());
                        }
                    }
                }
            }
            else
            {
                switch (platform_and_version.version)
                {
                case LevelVersion::Tomb1:
                    // TODO: What?
                    break;
                case LevelVersion::Tomb2:
                case LevelVersion::Tomb3:
                    if (auto name = read_tombpc_1_3(_files, level_ptr->filename()))
                    {
                        return name.value();
                    }
                    break;
                case LevelVersion::Tomb4:
                case LevelVersion::Tomb5:
                    if (auto name = read_tombpc_4_5(_files, level_ptr->filename()))
                    {
                        return name.value();
                    }
                    break;
                }
            }
        }

        // Mode 2: Hash lookup - can hash be calculated on load and stored?


        // Mode 3: give up
        const std::filesystem::path filename_path{ level_ptr->filename() };
        return filename_path.filename().string();
    }
}
