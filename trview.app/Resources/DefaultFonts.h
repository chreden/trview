#pragma once

namespace trview
{
    namespace graphics
    {
        struct IFontFactory;
        struct IDevice;
    }

    /// Loads the fonts that have been embedded in the resource file and puts them into the font storage provided.
    /// @param device The Direct3D device to use to load the fonts.
    /// @param font_factory The FontFactory instance to store the fonts in.
    void load_default_fonts(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IFontFactory>& font_factory);
}
