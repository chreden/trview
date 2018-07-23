#include "FontFactory.h"
#include "Font.h"
#include <external/DirectXTK/Inc/SpriteFont.h>

using namespace Microsoft::WRL;

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

        std::unique_ptr<Font> FontFactory::create_font(const std::wstring& font_face, float size, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment)
        {
            return nullptr;
        }
    }
}
