#include "ImageNode.h"
#include <trview.ui/Image.h>
#include "Sprite.h"
#include <trview.graphics/RenderTargetStore.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            ImageNode::ImageNode(const Microsoft::WRL::ComPtr<ID3D11Device>& device, Image* image)
                : WindowNode(device, image), _image(image)
            {
            }

            ImageNode::~ImageNode()
            {
            }

            void ImageNode::render_self(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, Sprite& sprite)
            {
                WindowNode::render_self(context, sprite);
                graphics::RenderTargetStore store(context);
                context->OMSetRenderTargets(1, _render_target_view.GetAddressOf(), nullptr);
                auto texture = _image->texture();
                if (texture.view)
                {
                    auto size = _image->size();
                    sprite.render(context, texture.view, 0, 0, size.width, size.height);
                }
            }
        }
    }
}