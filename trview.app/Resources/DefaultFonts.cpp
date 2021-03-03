#include "DefaultFonts.h"
#include "resource.h"
#include "ResourceHelper.h"
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/Device.h>

using namespace Microsoft::WRL;
using namespace DirectX;

namespace trview
{
    namespace
    {
        /// Load a specific font with the specified ID from the embedded resource file.
        /// @param device The Direct3D device to use to load the font.
        /// @param resource_id The integer ID of the font in the resource file.
        /// @returns The font loaded from the resource.
        std::shared_ptr<SpriteFont> load_font_from_resource(const graphics::Device& device, int resource_id)
        {
            auto resource_memory = get_resource_memory(resource_id, L"SPRITEFONT");
            return std::make_shared<SpriteFont>(device.device().Get(), resource_memory.data, resource_memory.size);
        }
    }

    /// Loads the fonts that have been embedded in the resource file and puts them into the font storage provided.
    /// @param device The Direct3D device to use to load the fonts.
    /// @param font_factory The FontFactory instance to store the fonts in.
    void load_default_fonts(const graphics::Device& device, graphics::IFontFactory& font_factory)
    {
        // Load some sort of manifest that contains the files to load.
        // For each font, load it with the given key.
        Resource texture_list = get_resource_memory(IDR_FONT_LIST, L"TEXT");

        auto contents = std::string(texture_list.data, texture_list.data + texture_list.size);
        std::stringstream stream(contents);

        while (!stream.eof())
        {
            std::string key;
            int resource_id = 0;
            if (!(stream >> key >> resource_id))
            {
                break;
            }

            font_factory.store(key, load_font_from_resource(device, resource_id));

            if (!std::getline(stream, key))
            {
                break;
            }
        }
    }
}
