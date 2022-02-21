#pragma once

namespace trview
{
    namespace graphics
    {
        struct IFontFactory;
        struct IDevice;
    }

    /// <summary>
    /// Loads the fonts that have been embedded in the resource file and puts them into the font storage provided.
    /// </summary>
    /// <param name="device">The Direct3D device to use to load the fonts.</param>
    /// <param name="font_factory">Location to store new fonts.</param>
    void load_default_fonts(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IFontFactory>& font_factory);
}
