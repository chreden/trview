#pragma once

#include <string>
#include <vector>
#include "../../Settings/FontSetting.h"

struct ImFont;

namespace trview
{
    struct IFonts
    {
        virtual ~IFonts() = 0;
        /// <summary>
        /// Get all of the installed fonts from the system or user.
        /// </summary>
        virtual std::vector<FontSetting> list_fonts() const = 0;
        /// <summary>
        /// Attempt to create a font based on the provided setting.
        /// </summary>
        virtual ImFont* add_font(const std::string& name, const FontSetting& setting) = 0;
        virtual ImFont* font(const std::string& name) const = 0;
        /// <summary>
        /// Get all loaded fonts.
        /// </summary>
        /// <returns></returns>
        virtual std::unordered_map<std::string, FontSetting> fonts() const = 0;
    };
}
