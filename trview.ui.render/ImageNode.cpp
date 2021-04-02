#include "ImageNode.h"
#include <trview.ui/Image.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            ImageNode::ImageNode(const std::shared_ptr<graphics::IDevice>& device, const graphics::IRenderTarget::SizeSource& render_target_source, Image* image)
                : WindowNode(device, render_target_source, image), _image(image)
            {
            }

            ImageNode::~ImageNode()
            {
            }

            void ImageNode::render_self(graphics::ISprite& sprite)
            {
                WindowNode::render_self(sprite);
                auto texture = _image->texture();
                if (texture.can_use_as_resource())
                {
                    auto context = _device->context();
                    graphics::RenderTargetStore rt_store(context);
                    graphics::ViewportStore vp_store(context);
                    graphics::SpriteSizeStore s_store(sprite, _render_target->size());
                    _render_target->apply();

                    auto size = _image->size();
                    sprite.render(texture, 0, 0, size.width, size.height);
                }
            }
        }
    }
}