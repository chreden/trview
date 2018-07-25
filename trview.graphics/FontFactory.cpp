#include "FontFactory.h"
#include "Font.h"
#include <external/DirectXTK/Inc/SpriteFont.h>

using namespace Microsoft::WRL;
using namespace std::string_literals;

namespace trview
{
    namespace graphics
    {
        FontFactory::FontFactory(const ComPtr<ID3D11Device>& device)
            : _device(device)
        {
        }

        FontFactory::~FontFactory()
        {
        }

        void FontFactory::store(const std::string& key, const std::shared_ptr<DirectX::SpriteFont>& font)
        {
            _cache.insert({ key, font });
        }

        std::unique_ptr<Font> FontFactory::create_font(const std::string& font_face, int size, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment) const
        {
            // Look for the font in the cache....
            const auto key = font_face + std::to_string(size);
            auto found = _cache.find(key);
            if (found == _cache.end())
            {
                throw std::runtime_error("Couldn't find font '" + key + "' in the cache");
            }

            // Create the font holder.
            return std::make_unique<Font>(found->second, text_alignment, paragraph_alignment);
        }
    }
}
