#include "LevelNameLookup.h"
#include "../ILevel.h"
#include <trlevel/ILevel.h>

#include <filesystem>
#include <spanstream>
#include <ranges>

#include <bcrypt.h>

#include <trlevel/Level_common.h>

namespace trview
{
    namespace
    {
        class TombScript final
        {
        public:
            // Load Tomb 2-3 script data.
            explicit TombScript(const std::vector<uint8_t>& data);

            enum class Opcode
            {
                Picture = 0,
                ListStart,
                ListEnd,
                FMV,
                Level,
                Cine,
                Complete,
                Demo,
                JumpToSeqence,
                End,
                Track,
                Sunset,
                LoadPic,
                DeadlyWater,
                RemoveWeapons,
                GameComplete,
                CutAngle,
                NoFloor,
                StartInv,
                StartAnim,
                Secrets,
                KillToComplete,
                RemoveAmmo
            };

            struct Operation
            {
                Opcode   opcode;
                uint16_t operand;
            };

            std::optional<int32_t> index(const std::string& level_filename) const;
            std::optional<std::string> name(const std::string& level_filename) const;
            std::optional<std::vector<Operation>> operations(const std::string& level_filename) const;
        private:

            struct ScriptLevel
            {
                std::string filename;
                std::string name;
                std::vector<Operation> operations;
            };
            
            std::vector<ScriptLevel> _levels;
        };

        TombScript::TombScript(const std::vector<uint8_t>& data)
        {
            std::basic_ispanstream<uint8_t> file{ std::span(data) };
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

            // Title
            _levels.push_back({});

            // Levels
            for (int32_t i = 0; i < num_levels; ++i)
            {
                _levels.push_back(ScriptLevel { .filename = level_paths[i], .name = level_names[i] });
            }

            const std::vector<uint16_t> sequence_offset = read_vector<uint16_t>(file, num_levels + 1);
            const uint16_t sequence_num_bytes = read<uint16_t>(file);
            sequence_num_bytes;

            for (int32_t i = 0; i <= num_levels; ++i)
            {
                std::vector<int32_t> items;

                Opcode opcode = static_cast<Opcode>(read<uint16_t>(file));
                while (opcode != Opcode::End)
                {
                    uint16_t operand = 0;
                    switch (opcode)
                    {
                    case Opcode::Picture:
                    case Opcode::ListStart:
                    case Opcode::ListEnd:
                    case Opcode::FMV:
                    case Opcode::Level:
                    case Opcode::Cine:
                    case Opcode::Demo:
                    case Opcode::JumpToSeqence:
                    case Opcode::Track:
                    case Opcode::LoadPic:
                    case Opcode::CutAngle:
                    case Opcode::NoFloor:
                    case Opcode::StartInv:
                    case Opcode::StartAnim:
                    case Opcode::Secrets:
                        operand = read<uint16_t>(file);
                        break;
                    default:
                        operand = 0;
                        break;
                    }

                    _levels[i].operations.push_back(Operation { .opcode = opcode, .operand = operand });
                    opcode = static_cast<Opcode>(read<uint16_t>(file));
                }

                if (opcode == Opcode::End)
                {
                    _levels[i].operations.push_back(Operation{ .opcode = opcode, .operand = 0 });
                }
            }
        }

        std::optional<int32_t> TombScript::index(const std::string& level_filename) const
        {
            const auto lower_filename = to_lowercase(level_filename);
            int32_t index = 0;
            for (const auto& level : _levels)
            {
                if (level.filename.contains(lower_filename))
                {
                    return index;
                }
                ++index;
            }
            return std::nullopt;
        }

        std::optional<std::string> TombScript::name(const std::string& level_filename) const
        {
            const auto lower_filename = to_lowercase(level_filename);
            for (const auto& level : _levels)
            {
                if (level.filename.contains(lower_filename))
                {
                    return level.name;
                }
            }
            return std::nullopt;
        }

        std::optional<std::vector<TombScript::Operation>> TombScript::operations(const std::string& level_filename) const
        {
            const auto lower_filename = to_lowercase(level_filename);
            for (const auto& level : _levels)
            {
                if (level.filename.contains(lower_filename))
                {
                    return level.operations;
                }
            }
            return std::nullopt;
        }

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

        struct LevelKeyValue
        {
            std::string key;
            std::string value;
        };

        std::vector<LevelKeyValue> find_level_names(const std::vector<uint8_t>& data)
        {
            const std::string strings_text = { std::from_range, data };

            std::vector<LevelKeyValue> levels;
            std::size_t current_position = strings_text.find("LVL_", 0);
            while (current_position != strings_text.npos)
            {
                const std::size_t end_of_key = strings_text.find('=', current_position);
                const std::size_t end_of_line = strings_text.find('\r', current_position);
                const std::string key = strings_text.substr(current_position + 4, end_of_key - current_position - 4);
                const std::string value = strings_text.substr(end_of_key + 1, end_of_line - end_of_key - 1);
                levels.push_back({ .key = key, .value = value });
                current_position = strings_text.find("LVL_", current_position + 1);
            }
            return levels;
        }

        std::optional<std::vector<LevelKeyValue>> load_strings_data(const std::shared_ptr<IFiles>& files, const std::filesystem::path& level_path)
        {
            if (const auto strings_data = files->load_file(level_path.parent_path() /= "../TEXT/EN/STRINGS.TXT"))
            {
                return find_level_names(strings_data.value());
            }

            if (const auto expansion_strings_data = files->load_file(level_path.parent_path() /= "../../TEXT/EN/STRINGS.TXT"))
            {
                return find_level_names(expansion_strings_data.value());
            }

            return std::nullopt;
        }

        std::optional<std::vector<uint8_t>> get_tombpc_data(const std::shared_ptr<IFiles>& files, const std::filesystem::path& level_path)
        {
            const auto tombpc = files->load_file(level_path.parent_path() /= "TOMBPC.DAT");
            if (tombpc)
            {
                return tombpc;
            }
            return files->load_file(level_path.parent_path() /= "trtla.dat");
        }

        std::optional<TombScript> load_script(const std::shared_ptr<IFiles>& files, const std::filesystem::path& level_path)
        {
            const auto data = get_tombpc_data(files, level_path);
            if (data)
            {
                return TombScript(data.value());
            }
            return std::nullopt;
        }

        std::optional<ILevelNameLookup::Name> read_tombpc_1_3(const std::shared_ptr<IFiles>& files, const std::string& level_filename)
        {
            try
            {
                const std::filesystem::path level_path{ level_filename };
                if (const auto script = load_script(files, level_path))
                {
                    if (const auto name = script->name(level_path.filename().string()))
                    {
                        return ILevelNameLookup::Name{ .name = name.value(), .index = script->index(level_path.filename().string()) };
                    }
                }
            }
            catch(...)
            {
            }
            return std::nullopt;
        }

        std::optional<std::vector<int32_t>> read_tombpc_1_3_bonus_items(const std::shared_ptr<IFiles>& files, const std::string& level_filename)
        {
            try
            {
                const std::filesystem::path level_path{ level_filename };
                if (const auto script = load_script(files, level_path))
                {
                    if (const auto operations = script->operations(level_path.filename().string()))
                    {
                        std::vector<int32_t> items;
                        for (const auto& operation : operations.value())
                        {
                            if (operation.opcode == TombScript::Opcode::StartInv)
                            {
                                items.push_back(static_cast<int32_t>(operation.operand));
                            }
                        }
                        return items;
                    }
                }
            }
            catch(...)
            {
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
                std::basic_ispanstream<uint8_t> file{ std::span(*script_data) };
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
                std::basic_ispanstream<uint8_t> file{ std::span(*data) };
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

        std::optional<ILevelNameLookup::Name> read_english_4_5(const std::shared_ptr<IFiles>& files, const std::string& level_filename, trlevel::LevelVersion version)
        {
            const std::optional<uint8_t> string_index = string_index_for_level(files, level_filename, version);
            if (string_index.has_value())
            {
                const auto name = string_at_index(files, level_filename, string_index.value());
                if (name)
                {
                    return ILevelNameLookup::Name{ .name = name.value(), .index = string_index };
                }
            }
            return std::nullopt;
        }

        std::optional<int> level_index_from_gameflow(const std::shared_ptr<IFiles>& files, const std::filesystem::path& gameflow_path, const std::string& filename)
        {
            int index = 0;
            if (auto gameflow = files->load_file(gameflow_path))
            {
                auto json = nlohmann::json::parse(*gameflow, nullptr, true, true, true);
                if (json.contains("levels"))
                {
                    for (const auto& level : json["levels"].items())
                    {
                        if (level.value().contains("path"))
                        {
                            const std::string level_path = level.value()["path"].get<std::string>();
                            if (to_lowercase(level_path) == to_lowercase("data/" + filename))
                            {
                                return index;
                            }
                        }
                        ++index;
                    }
                }
            }
            return std::nullopt;
        }

        std::optional<ILevelNameLookup::Name> level_name_from_strings(const std::shared_ptr<IFiles>& files, const std::filesystem::path& strings_path, int index)
        {
            if (auto strings = files->load_file(strings_path))
            {
                auto json = nlohmann::json::parse(*strings, nullptr, true, true, true);
                const auto& level = json["levels"].at(index);
                return ILevelNameLookup::Name{ .name = level["title"].get<std::string>(), .index = index };
            }
            return std::nullopt;
        }

        std::string trx_filter_for_level(trlevel::LevelVersion version)
        {
            return std::format("tr{}*", static_cast<int>(version));
        }

        const std::map<std::string, std::vector<int32_t>> remastered_bonus_items
        {
            { "wall.tr2", { 6, 13, 13, 15 } },
            { "boat.tr2", { 9, 9, 9, 9 } },
            { "venice.tr2", { 8, 8, 8, 8 } },
            { "opera.tr2", { 3, 10, 10, 10, 10 } },
            { "rig.tr2", { 3, 10, 10 } },
            { "platform.tr2", { 10, 10, 10, 10 } },
            { "unwater.tr2", { 11, 11, 11, 11 } },
            { "keel.tr2", { 6, 13, 13 } },
            { "living.tr2", { 12, 12, 12, 12 } },
            { "deck.tr2", { 13, 13, 13, 13 } },
            { "skidoo.tr2", { 10, 10, 10, 10 } },
            { "monastry.tr2", { 12, 12, 12, 12 } },
            { "catacomb.tr2", { 13, 13, 12, 12 } },
            { "icecave.tr2", { 13, 13, 13, 13 } },
            { "emprtomb.tr2", { 10, 10, 10, 10, 10, 10, 10, 10 } },
            { "floating.tr2", { 13, 13, 13, 13, 13, 13, 13, 13 } }
        };
    }

    ILevelNameLookup::~ILevelNameLookup()
    {
    }

    LevelNameLookup::LevelNameLookup(const std::shared_ptr<IFiles>& files, const std::string& level_hashes_json)
        : _files(files)
    {
        const auto json = nlohmann::json::parse(level_hashes_json.begin(), level_hashes_json.end(), nullptr, true, true, true);
        for (const auto& [key, value] : json.items())
        {
            _hashes.insert({ key, value.at("name").get<std::string>() });
        }
    }

    std::optional<ILevelNameLookup::Name> LevelNameLookup::lookup(const std::weak_ptr<ILevel>& level) const
    {
        const auto level_ptr = level.lock();
        if (!level_ptr)
        {
            return std::nullopt;
        }
        return get_name(level_ptr->filename(), level_ptr->platform_and_version(), level_ptr->hash());
    }

    std::optional<ILevelNameLookup::Name> LevelNameLookup::lookup(const std::weak_ptr<trlevel::ILevel>& level) const
    {
        const auto level_ptr = level.lock();
        if (!level_ptr)
        {
            return std::nullopt;
        }
        return get_name(level_ptr->filename(), level_ptr->platform_and_version(), level_ptr->hash());
    }

    std::vector<int32_t> LevelNameLookup::bonus_items(const std::weak_ptr<ILevel>& level) const
    {
        const auto level_ptr = level.lock();
        if (!level_ptr)
        {
            return {};
        }
        return get_bonus_items(level_ptr->filename(), level_ptr->platform_and_version(), level_ptr->hash());
    }

    bool LevelNameLookup::is_trx() const
    {
        return false;
    }

    std::optional<ILevelNameLookup::Name> LevelNameLookup::check_remastered(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const
    {
        if (!platform_and_version.remastered)
        {
            return std::nullopt;
        }

        const std::filesystem::path level_path{ filename };
        const std::string level_stem = level_path.stem().string();
        if (const auto strings_data = load_strings_data(_files, level_path))
        {
            for (std::size_t index = 0; index < strings_data->size(); ++index)
            {
                const auto& entry = (*strings_data)[index];
                if (to_lowercase(entry.key) == to_lowercase(level_stem))
                {
                    return ILevelNameLookup::Name{ .name = entry.value, .index = static_cast<int32_t>(index) };
                }
            }
        }

        return std::nullopt;
    }

    std::optional<ILevelNameLookup::Name> LevelNameLookup::check_trx(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const
    {
        using namespace trlevel;

        if (platform_and_version.platform == Platform::PC)
        {
            const std::filesystem::path file_path{ filename };
            auto dirs = _files->get_directories(
                std::format("{}/{}", file_path.parent_path().parent_path().string(), "/cfg"),
                trx_filter_for_level(platform_and_version.version));

            for (const auto& dir : dirs)
            {
                try
                {
                    std::filesystem::path gameflow_path = dir.path;
                    gameflow_path += "/gameflow.json5";
                    std::optional<int> level_index = level_index_from_gameflow(_files, gameflow_path, file_path.filename().string());
                    if (level_index)
                    {
                        std::filesystem::path strings_path = gameflow_path;
                        strings_path.replace_filename("strings.json5");
                        return level_name_from_strings(_files, strings_path, *level_index);
                    }
                }
                catch (const nlohmann::detail::parse_error& e)
                {
                    OutputDebugStringA(e.what());
                }
            }
        }

        return std::nullopt;
    }

    std::optional<std::vector<int32_t>> LevelNameLookup::check_remastered_bonus_items(const std::string& filename, trlevel::PlatformAndVersion platform_and_version) const
    {
        if (!platform_and_version.remastered)
        {
            return std::nullopt;
        }

        const std::filesystem::path file_path{ filename };
        const auto found = remastered_bonus_items.find(to_lowercase(file_path.filename().string()));
        if (found != remastered_bonus_items.end())
        {
            return found->second;
        }
        return {};
    }

    std::vector<int32_t> LevelNameLookup::get_bonus_items(const std::string& filename, trlevel::PlatformAndVersion platform_and_version, const std::string& hash) const
    {
        using namespace trlevel;

        // Mode 1: Check in the files (TOMBPC, English.DAT, etc)
        if (platform_and_version.platform == Platform::PC)
        {
            if (auto remastered = check_remastered_bonus_items(filename, platform_and_version))
            {
                return remastered.value();
            }

            switch (platform_and_version.version)
            {
            case LevelVersion::Tomb2:
            case LevelVersion::Tomb3:
                if (auto bonus = read_tombpc_1_3_bonus_items(_files, filename))
                {
                    return bonus.value();
                }
                return {};
            case LevelVersion::Tomb4:
            case LevelVersion::Tomb5:
                return {};
            }
        }

        // Mode 2: Hash lookup - can hash be calculated on load and stored?
        const auto found = _hashes.find(hash);
        if (found != _hashes.end())
        {
            return {};
        }

        return {};
    }

    std::optional<ILevelNameLookup::Name> LevelNameLookup::get_name(const std::string& filename, trlevel::PlatformAndVersion platform_and_version, const std::string& hash) const
    {
        using namespace trlevel;

        // Mode 1: Check in the files (TOMBPC, English.DAT, etc)
        if (platform_and_version.platform == Platform::PC)
        {
            if (auto remastered = check_remastered(filename, platform_and_version))
            {
                return remastered;
            }

            if (auto trx = check_trx(filename, platform_and_version))
            {
                return trx;
            }

            switch (platform_and_version.version)
            {
            case LevelVersion::Tomb2:
            case LevelVersion::Tomb3:
                if (auto name = read_tombpc_1_3(_files, filename))
                {
                    return name;
                }
                break;
            case LevelVersion::Tomb4:
            case LevelVersion::Tomb5:
                if (auto name = read_english_4_5(_files, filename, platform_and_version.version))
                {
                    return name;
                }
                break;
            }
        }

        // Mode 2: Hash lookup - can hash be calculated on load and stored?
        const auto found = _hashes.find(hash);
        if (found != _hashes.end())
        {
            return ILevelNameLookup::Name{ .name = found->second };
        }

        return std::nullopt;
    }
}
