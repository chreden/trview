#include "LevelNameLookup.h"
#include "../ILevel.h"

#include <filesystem>
#include <spanstream>
#include <ranges>

#include <bcrypt.h>

#include <trlevel/Level_common.h>

namespace trview
{
    namespace
    {
        uint8_t tr4_opcode_size(uint8_t opcode)
        {
            switch (opcode)
            {
                case 0x80: return 1;
                case 0x81: return 5;
                case 0x82: return 4;
                case 0x83: return 0;
                case 0x84: case 0x85: case 0x86: case 0x87: case 0x88: return 1;
                case 0x89: case 0x8A: return 4;
                case 0x8B: case 0x8C: return 1;
                case 0x8D: return 9;
                case 0x8E: return 5;
                case 0x8F: return 3;
                case 0x90: return 1;
                case 0x91: return 25;
                case 0x92: return 1;
                default: 
                {
                    if (opcode >= 0x93 && opcode <= 0xd9)
                    {
                        return 14;
                    }
                }
            }
            return 0;
        }

        uint8_t tr5_opcode_size(uint8_t opcode)
        {
            switch (opcode)
            {
                case 0x80: return 1;
                case 0x81: return 5;
                case 0x82: return 4;
                case 0x83: return 0;
                case 0x84: case 0x85: case 0x86: case 0x87: case 0x88: return 1;
                case 0x89: case 0x8A: return 4;
                case 0x8B: case 0x8C: return 1;
                case 0x8D: return 9;
                case 0x8E: return 5;
                case 0x8F: return 3;
                case 0x90: return 1;
                case 0x91: return 1;
                case 0xD9: case 0xDA: return 2;
                default:
                {
                    if (opcode >= 0x92 && opcode <= 0xd8)
                    {
                        return 14;
                    }
                }
            }
            return 0;
        }

        uint8_t opcode_size(uint8_t opcode, trlevel::LevelVersion version)
        {
            return version == trlevel::LevelVersion::Tomb4 ? tr4_opcode_size(opcode) : tr5_opcode_size(opcode);
        }

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

        std::optional<uint8_t> string_index_for_level(const std::shared_ptr<IFiles>& files, const std::string& level_filename, trlevel::LevelVersion version)
        {
            const std::filesystem::path level_path{ level_filename };
            const auto script_data = files->load_file(level_path.parent_path() /= "../script.DAT");
            if (!script_data)
            {
                return std::nullopt;
            }

            try
            {
                std::basic_ispanstream<uint8_t> file{ { *script_data } };
                file.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
                using namespace trlevel;

                skip(file, 9);
                uint8_t num_levels = read<uint8_t>(file);
                skip(file, 2);
                uint16_t level_path_length = read<uint16_t>(file);
                uint16_t level_block_length = read<uint16_t>(file);
                level_block_length;
                skip(file, 40);

                const auto path_offsets = read_vector<uint16_t>(file, num_levels);
                const auto level_path_data = read_vector<uint8_t>(file, level_path_length);
                const auto level_data_offsets = read_vector<uint16_t>(file, num_levels);
                const auto levels = path_offsets
                    | std::views::transform([&](auto offset) { return std::string(reinterpret_cast<const char*>(&level_path_data[offset])); })
                    | std::views::transform(to_lowercase)
                    | std::ranges::to<std::vector>();

                const auto lower_filename = to_lowercase(level_path.stem().string());
                const auto found_index = std::ranges::find_if(levels, [&](auto& f) { return f.contains(lower_filename); });
                if (found_index != levels.end())
                {
                    uint32_t level_number = static_cast<uint32_t>(found_index - levels.begin());
                    uint16_t required_offset = level_data_offsets[level_number];

                    skip(file, required_offset);

                    uint8_t opcode = read<uint8_t>(file);
                    while (opcode != 0x83)
                    {
                        switch (opcode)
                        {
                            case 0x81:
                            {
                                return read<uint8_t>(file);
                            }
                            default:
                            {
                                skip(file, opcode_size(opcode, version));
                                break;
                            }
                        }
                        opcode = read<uint8_t>(file);
                    }
                }
            }
            catch (...)
            {
            }

            return std::nullopt;
        }

        std::optional<std::string> string_at_index(const std::shared_ptr<IFiles>& files, const std::string& level_filename, uint16_t string_index)
        {
            const std::filesystem::path level_path{ level_filename };
            const auto data = files->load_file(level_path.parent_path() /= "../ENGLISH.DAT");
            if (!data)
            {
                return std::nullopt;
            }

            try
            {
                std::basic_ispanstream<uint8_t> file{ { *data } };
                file.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
                using namespace trlevel;

                uint16_t num_generic_strings = read<uint16_t>(file);
                uint16_t num_psx_strings = read<uint16_t>(file);
                uint16_t num_pc_strings = read<uint16_t>(file);

                skip(file, 6); // lengths.

                const auto offsets = read_vector<uint16_t>(file, num_generic_strings + num_psx_strings + num_pc_strings);
                if (string_index >= offsets.size())
                {
                    return std::nullopt;
                }

                skip(file, offsets[string_index]);

                std::vector<char> string_data;
                char character = read<uint8_t>(file);
                while (character != 0)
                {
                    string_data.push_back(character ^ 0xa5);
                    character = read<uint8_t>(file);
                }
                return std::string(std::from_range, string_data);
            }
            catch (...)
            {
            }

            return std::nullopt;
        }

        std::optional<std::string> read_english_4_5(const std::shared_ptr<IFiles>& files, const std::string& level_filename, trlevel::LevelVersion version)
        {
            const std::optional<uint8_t> string_index = string_index_for_level(files, level_filename, version);
            if (string_index.has_value())
            {
                return string_at_index(files, level_filename, string_index.value());
            }
            return std::nullopt;
        }
    }

    ILevelNameLookup::~ILevelNameLookup()
    {
    }

    LevelNameLookup::LevelNameLookup(const std::shared_ptr<IFiles>& files, const std::string& level_hashes_json)
        : _files(files)
    {
        const auto json = nlohmann::json::parse(level_hashes_json.begin(), level_hashes_json.end());
        for (const auto& [key, value] : json.items())
        {
            _hashes.insert({ key, value.at("name").get<std::string>() });
        }
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
                case LevelVersion::Tomb2:
                case LevelVersion::Tomb3:
                    if (auto name = read_tombpc_1_3(_files, level_ptr->filename()))
                    {
                        return name.value();
                    }
                    break;
                case LevelVersion::Tomb4:
                case LevelVersion::Tomb5:
                    if (auto name = read_english_4_5(_files, level_ptr->filename(), platform_and_version.version))
                    {
                        return name.value();
                    }
                    break;
                }
            }
        }

        // Mode 2: Hash lookup - can hash be calculated on load and stored?
        const auto found = _hashes.find(level_ptr->hash());
        if (found != _hashes.end())
        {
            return found->second;
        }

        // Mode 3: give up
        const std::filesystem::path filename_path{ level_ptr->filename() };
        return filename_path.filename().string();
    }
}
