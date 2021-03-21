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
                explicit ImageNode(const graphics::IDevice& device, Image* image);
                virtual ~ImageNode();
            protected:
                virtual void render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::Sprite& sprite) override;
            private:
                Image * _image;
            };
        }
    }
}