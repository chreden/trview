#include "Fonts.h"
#include <format>
#include <ranges>

namespace trview
{
    namespace
    {
        std::vector<FontSetting> load_for_key(HKEY key_search)
        {
            std::vector<FontSetting> available_fonts;
            HKEY key = nullptr;
            if (ERROR_SUCCESS == RegOpenKeyEx(key_search, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &key))
            {
                DWORD value_count = 0;
                DWORD max_value_length = 0;
                std::vector<wchar_t> value(16383, 0);

                LRESULT result = RegQueryInfoKey(key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &value_count, &max_value_length, nullptr, nullptr, nullptr);
                for (uint32_t i = 0; i < value_count; ++i)
                {
                    DWORD value_length = 16383;
                    value[0] = '\0';

                    DWORD data_length = 0;
                    result = RegEnumValue(key, i, &value[0], &value_length, NULL, NULL, NULL, &data_length);

                    value_length = 16383;

                    std::vector<uint8_t> data_value(data_length, 0);
                    result = RegEnumValue(key, i, &value[0], &value_length, NULL, NULL, &data_value[0], &data_length);

                    wchar_t* value_string = reinterpret_cast<wchar_t*>(&data_value[0]);
                    if (value_string &&
                        std::wstring(&value[0]).ends_with(L" (TrueType)"))
                    {
                        const auto name = to_utf8(&value[0]);
                        available_fonts.push_back(
                            {
                                .name = name.substr(0, name.length() - 11),
                                .filename = to_utf8(value_string)
                            });
                    }
                }
            }

            RegCloseKey(key);
            return available_fonts;
        }
    }

    IFonts::~IFonts()
    {
    }

    Fonts::Fonts(const std::shared_ptr<IFiles>& files)
        : _files(files)
    {
    }

    std::vector<FontSetting> Fonts::list_fonts() const
    {
        std::vector<FontSetting> available_fonts = load_for_key(HKEY_LOCAL_MACHINE);
        available_fonts.append_range(load_for_key(HKEY_CURRENT_USER));
        std::ranges::sort(available_fonts, [](auto&& l, auto&& r) { return l.name < r.name; });
        return available_fonts;
    }

    std::optional<IFonts::Font> Fonts::add_font(const std::string& name, const FontSetting& setting)
    {
        auto new_font = add_font(setting);
        if (!new_font)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();
            rebuild_fonts();
            return std::nullopt;
        }

        IFonts::Font font = { .setting = setting, .font = new_font };
        _fonts[name] = font;
        return font;
    }

    std::optional<IFonts::Font> Fonts::font(const std::string& name) const
    {
        const auto found = _fonts.find(name);
        if (found == _fonts.end())
        {
            return std::nullopt;
        }
        return found->second;
    }

    std::unordered_map<std::string, FontSetting> Fonts::fonts() const
    {
        return _fonts |
            std::views::transform([](auto&& f) -> std::pair<std::string, FontSetting> { return { f.first, f.second.setting }; }) |
            std::ranges::to<std::unordered_map>();
    }

    ImFont* Fonts::add_font(const FontSetting& setting)
    {
        const std::string font_path = setting.filename.contains('\\') ?
            setting.filename :
            std::format("{}\\{}", _files->fonts_directory(), setting.filename);
        ImGuiIO& io = ImGui::GetIO();
        return io.Fonts->AddFontFromFileTTF(font_path.c_str());
    }

    void Fonts::rebuild_fonts()
    {
        for (auto& [_, font] : _fonts)
        {
            font.font = add_font(font.setting);
        }
    }
}
