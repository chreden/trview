#pragma once

#include "ILauWindow.h"
#include "CollapsiblePanel.h"
#include <trview.ui/ILoader.h>

namespace trview
{
    namespace ui
    {
        class Button;
        class Listbox;
    }

    struct IDialogs;

    class LauWindow final : public ILauWindow, public CollapsiblePanel
    {
    public:
        struct Names
        {
            inline static const std::string open = "Open";
            inline static const std::string sections = "Sections";
        };

        explicit LauWindow(const graphics::IDeviceWindow::Source& device_window_source,
            const ui::render::IRenderer::Source& renderer_source,
            const ui::IInput::Source& input_source,
            const std::shared_ptr<IDialogs>& dialogs,
            const Window& parent,
            const std::shared_ptr<ui::ILoader>& loader);
        virtual ~LauWindow() = default;
        virtual void render(bool vsync) override;
    private:
        void bind_controls();
        void load_file(const std::string& filename);

        std::shared_ptr<IDialogs> _dialogs;
        ui::Button* _open{ nullptr };
        ui::Listbox* _sections{ nullptr };
    };
}
