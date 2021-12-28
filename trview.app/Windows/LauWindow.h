#pragma once

#include "ILauWindow.h"
#include "CollapsiblePanel.h"
#include <trview.ui/ILoader.h>

namespace trview
{
    class LauWindow final : public ILauWindow, public CollapsiblePanel
    {
    public:
        explicit LauWindow(const graphics::IDeviceWindow::Source& device_window_source,
            const ui::render::IRenderer::Source& renderer_source,
            const ui::IInput::Source& input_source,
            const Window& parent,
            const std::shared_ptr<ui::ILoader>& loader);
        virtual ~LauWindow() = default;
        virtual void render(bool vsync) override;
    private:
        void bind_controls();

    };
}
