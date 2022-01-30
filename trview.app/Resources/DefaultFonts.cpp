#include "DefaultFonts.h"
#include <trview.common/Resources.h>
#include <trview.graphics/IFontFactory.h>
#include <trview.graphics/IDevice.h>
#include <trview.common/Strings.h>

using namespace Microsoft::WRL;
using namespace DirectX;

namespace trview
{
    namespace ui
    {
        namespace render
        {
            namespace
            {
                /// <summary>
                /// Load a specific font with the specified name from the embedded resource file.
                /// </summary>
                /// <param name="device">The Direct3D device to use to load the font.</param>
                /// <param name="resource_name">The name of the resource in the resource file.</param>
                /// <returns>The font loaded from the resource.</returns>
                std::shared_ptr<SpriteFont> load_font_from_resource(const graphics::IDevice& device, const std::wstring& resource_name)
                {
                    auto resource_memory = get_resource_memory(resource_name, L"SPRITEFONT");
                    return std::make_shared<SpriteFont>(device.device().Get(), resource_memory.data, resource_memory.size);
                }
            }

            void load_default_fonts(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IFontFactory>& font_factory)
            {
                for (const auto& name : get_resource_names_of_type(L"SPRITEFONT"))
                {
                    font_factory->store(to_utf8(name), load_font_from_resource(*device, name));
                }
            }
        }
    }
}
