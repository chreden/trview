#include "LevelNameLookup.h"
#include "../ILevel.h"

#include <filesystem>

namespace trview
{
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
                const auto strings_path = level_path.parent_path() /= "../TEXT/EN/STRINGS.TXT";
                if (const auto strings_data = _files->load_file(strings_path))
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
                    // TODO: TOMBPC.DAT
                    break;
                case LevelVersion::Tomb4:
                case LevelVersion::Tomb5:
                    // TODO: ENGLISH.DAT
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
