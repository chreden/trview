#include "LauWindow.h"
#include "../Resources/resource.h"
#include <trview.common/Windows/IDialogs.h>
#include <trview.ui/Button.h>
#include <trview.ui/Listbox.h>
#include <trview.lau/drm.h>

namespace trview
{
    using namespace ui;

    namespace
    {
        Listbox::Item make_item(const std::wstring& name, const std::wstring& value)
        {
            return Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };
    }

    ILauWindow::~ILauWindow()
    {
    }

    LauWindow::LauWindow(const graphics::IDeviceWindow::Source& device_window_source,
        const ui::render::IRenderer::Source& renderer_source,
        const ui::IInput::Source& input_source,
        const std::shared_ptr<IDialogs>& dialogs,
        const trview::Window& parent,
        const std::shared_ptr<ui::ILoader>& loader)
        : CollapsiblePanel(device_window_source, renderer_source(Size(600, 600)), parent, L"trview.lau", L"LAU Development", input_source,
             Size(600, 600), loader->load_from_resource(IDR_UI_LAU_WINDOW)), _dialogs(dialogs)
    {
        bind_controls();
    }

    void LauWindow::render(bool vsync)
    {
        CollapsiblePanel::render(vsync);
    }

    void LauWindow::bind_controls()
    {
        _open = _ui->find<Button>(Names::open);
        _token_store += _open->on_click += [&]()
        {
            const auto result = _dialogs->open_file(L"Open .DRM file", { { L"All TR LAU files", { L"*.drm" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
            if (result.has_value())
            {
                load_file(result.value().filename);
            }
        };

        _sections = _ui->find<Listbox>(Names::sections);
    }

    void LauWindow::load_file(const std::string& filename)
    {
        try
        {
            std::vector<Listbox::Item> items;
            auto drm = lau::load_drm(to_utf16(filename));
            for (const auto section : drm->sections)
            {
                items.push_back({{ { L"#", std::to_wstring(section.index) },
                                   { L"Type", section_type_to_string(section.header.type) } }});
            }
            _sections->set_items(items);
        }
        catch(const std::exception&)
        {
            _dialogs->message_box(window(), L"Failed to load DRM file", L"Error", IDialogs::Buttons::OK);
        }
    }
}
