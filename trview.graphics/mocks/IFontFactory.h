#pragma once

#include "../IFontFactory.h"
#include "../IFont.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            class MockFontFactory final : public IFontFactory
            {
            public:
                MOCK_METHOD(void, store, (const std::string&, const std::shared_ptr<DirectX::SpriteFont>&));
                MOCK_METHOD(std::unique_ptr<graphics::IFont>, create_font, (const std::string&, int, graphics::TextAlignment, graphics::ParagraphAlignment), (const));
            };
        }
    }
}
