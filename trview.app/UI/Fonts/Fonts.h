#pragma once

#include "IFonts.h"
#include <trview.common/IFiles.h>

namespace trview
{
    class Fonts : public IFonts
    {
    public:
        explicit Fonts(const std::shared_ptr<IFiles>& files);
        virtual ~Fonts() = default;
        std::vector<FontSetting> list_fonts() const override;
        ImFont* add_font(const std::string& name, const FontSetting& setting) override;
    private:
        std::shared_ptr<IFiles> _files;
    };
}
