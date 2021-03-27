#pragma once

#include <memory>
#include <trview.ui/Control.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            struct IRenderer
            {
                using RendererSource = std::function<std::unique_ptr<IRenderer>(const Size&)>;

                virtual ~IRenderer() = 0;
                virtual void load(Control* control) = 0;
                virtual void render() = 0;
                virtual void set_host_size(const Size& size) = 0;
            };
        }
    }
}
