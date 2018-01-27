#pragma once

#include "WindowNode.h"

namespace trview
{
    namespace ui
    {
        class Image;

        namespace render
        {
            class ImageNode : public WindowNode
            {
            public:
                explicit ImageNode(CComPtr<ID3D11Device> device, Image* image);
                virtual ~ImageNode();
            protected:
                virtual void render_self(CComPtr<ID3D11DeviceContext> context, Sprite& sprite) override;
            private:
                Image * _image;
            };
        }
    }
}