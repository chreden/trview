#pragma once

#include <trview.graphics/Device.h>

namespace trview
{
    namespace graphics
    {
        class FontFactory;
    }

    /// Loads the fonts that have been embedded in the resource file and puts them into the font storage provided.
    /// @param device The Direct3D device to use to load the fonts.
    /// @param font_factory The FontFactory instance to store the fonts in.
    void load_default_fonts(const graphics::Device& device, graphics::FontFactory& font_factory);
}
