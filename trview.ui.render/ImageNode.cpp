#include "ImageNode.h"
#include <trview.ui/Image.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            ImageNode::ImageNode(const graphics::IDevice& device, Image* image)
                : WindowNode(device, image), _image(image)
            {
            }

            ImageNode::~ImageNode()
            {
            }

            void ImageNode::render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, graphics::ISprite& sprite)
            {
                WindowNode::render_self(context, sprite);
                auto texture = _image->texture();
                if (texture.can_use_as_resource())
                {
                    graphics::RenderTargetStore rt_store(context);
                    graphics::ViewportStore vp_store(context);
                    graphics::SpriteSizeStore s_store(sprite, _render_target->size());
                    _render_target->apply(context);

                    auto size = _image->size();
                    sprite.render(texture, 0, 0, size.width, size.height);
                }
            }
        }
    }
}