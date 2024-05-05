#pragma once

#include <unordered_map>

#include <trview.common/IFiles.h>

#include "IFonts.h"
#include "../IImGuiBackend.h"

namespace trview
{
    class Fonts : public IFonts
    {
    public:
        explicit Fonts(const std::shared_ptr<IFiles>& files, const std::shared_ptr<IImGuiBackend>& imgui_backend);
        virtual ~Fonts() = default;
        std::vector<FontSetting> list_fonts() const override;
        ImFont* add_font(const std::string& name, const FontSetting& setting) override;
        ImFont* font(const std::string& name) const override;
    private:
        ImFont* add_font(const FontSetting& setting);
        void rebuild_fonts();

        struct Font
        {
            FontSetting setting;
            ImFont* font{ nullptr };
        };

        std::shared_ptr<IFiles> _files;
        std::unordered_map<std::string, Font> _fonts;
        std::shared_ptr<IImGuiBackend> _imgui_backend;
    };
}
