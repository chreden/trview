#pragma once

#include <unordered_map>

#include <trview.common/IFiles.h>

#include "IFonts.h"

namespace trview
{
    class Fonts : public IFonts
    {
    public:
        explicit Fonts(const std::shared_ptr<IFiles>& files);
        virtual ~Fonts() = default;
        std::vector<FontSetting> list_fonts() const override;
        std::optional<Font> add_font(const std::string& name, const FontSetting& setting) override;
        std::optional<Font> font(const std::string& name) const override;
        std::unordered_map<std::string, FontSetting> fonts() const override;
    private:
        ImFont* add_font(const FontSetting& setting);
        void rebuild_fonts();

        std::shared_ptr<IFiles> _files;
        std::unordered_map<std::string, Font> _fonts;
    };
}
