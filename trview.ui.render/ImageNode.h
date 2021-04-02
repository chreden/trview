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
                explicit ImageNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, Image* image);
                virtual ~ImageNode();
            protected:
                virtual void render_self(graphics::ISprite& sprite) override;
            private:
                Image * _image;
            };
        }
    }
}