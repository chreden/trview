#pragma once

#include "ILauWindow.h"
#include "CollapsiblePanel.h"
#include <trview.ui/ILoader.h>
#include <trview.lau/drm.h>
#include <trview.lau/IDrmLoader.h>

namespace trview
{
    namespace ui
    {
        class Button;
        class Listbox;
        class Control;
        class Image;
        class Label;
    }

    struct IDialogs;

    class LauWindow final : public ILauWindow, public CollapsiblePanel
    {
    public:
        struct Names
        {
            inline static const std::string open = "Open";
            inline static const std::string sections = "Sections";
            inline static const std::string texture_panel = "texture_panel";
            inline static const std::string texture = "texture";
            inline static const std::string texture_stats = "texture_stats";
            inline static const std::string filename = "filename";
        };

        explicit LauWindow(const graphics::IDeviceWindow::Source& device_window_source,
            const ui::render::IRenderer::Source& renderer_source,
            const ui::IInput::Source& input_source,
            const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<lau::IDrmLoader>& drm_loader,
            const Window& parent,
            const std::shared_ptr<ui::ILoader>& loader);
        virtual ~LauWindow() = default;
        virtual void render(bool vsync) override;
    private:
        void bind_controls();
        void load_file(const std::string& filename);
        void load_section(uint32_t index);

        std::shared_ptr<IDialogs> _dialogs;
        ui::Button* _open{ nullptr };
        ui::Listbox* _sections{ nullptr };
        ui::Control* _texture_panel{ nullptr };
        ui::Image* _texture{ nullptr };
        ui::Listbox* _texture_stats{ nullptr };
        ui::Label* _filename{ nullptr };
        std::unique_ptr<lau::Drm> _drm;
        std::shared_ptr<lau::IDrmLoader> _drm_loader;
    };
}
