#include "FontFactory.h"
#include "Font.h"
#include <stdexcept>
#include <trview.common/Strings.h>

using namespace Microsoft::WRL;
using namespace std::string_literals;

namespace trview
{
    namespace graphics
    {
        IFontFactory::~IFontFactory()
        {
        }

        void FontFactory::store(const std::string& key, const std::shared_ptr<DirectX::SpriteFont>& font)
        {
            _cache.insert({ to_lowercase(key), font });
        }

        std::unique_ptr<IFont> FontFactory::create_font(const std::string& font_face, int size, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment) const
        {
            // Look for the font in the cache....
            const auto key = to_lowercase(font_face) + std::to_string(size);
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
