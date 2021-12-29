#include "LauWindow.h"
#include "../Resources/resource.h"
#include <trview.common/Windows/IDialogs.h>
#include <trview.ui/Button.h>
#include <trview.ui/Listbox.h>

#include <external/DirectXTK/Src/DDS.h>
#include <external/DirectXTK/Inc/DDSTextureLoader.h>

namespace trview
{
    using namespace ui;

    namespace
    {
        Listbox::Item make_item(const std::wstring& name, const std::wstring& value)
        {
            return Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };

        graphics::Texture create_texture(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const lau::Texture& texture)
        {
            using namespace Microsoft::WRL;
            using namespace DirectX;

            graphics::Texture output;

            ComPtr<ID3D11Resource> resource;
            ComPtr<ID3D11ShaderResourceView> resource_view;

            DDS_HEADER header;
            memset(&header, 0, sizeof(header));
            header.size = sizeof(header);
            header.flags = 0x1 | 0x2 | 0x4 | 0x1000 | 0x80000;
            header.height = texture.height;
            header.width = texture.width;
            header.pitchOrLinearSize = texture.data.size();
            header.ddspf.size = sizeof(header.ddspf);
            // header.ddspf.dwFlags = 0x1; // Alpha?
            header.ddspf.flags = 4;
            header.ddspf.fourCC = 827611204;
            header.caps = 4096;

            std::vector<uint8_t> final_data(4 + sizeof(header) + texture.data.size(), 0u);
            uint32_t magic = 542327876u;
            memcpy(&final_data[0], &magic, sizeof(magic));
            memcpy(&final_data[4], &header, sizeof(header));
            memcpy(&final_data[4 + sizeof(header)], &texture.data[0], texture.data.size());

            HRESULT hr = DirectX::CreateDDSTextureFromMemory(device.Get(), &final_data[0], final_data.size(), resource.GetAddressOf(), resource_view.GetAddressOf());

            // ComPtr<ID3D11Texture2D> texture_resource;
            // resource.As<ID3D11Texture2D>(texture_resource.GetAddressOf());
            ComPtr<ID3D11Texture2D> texture_resource;
            resource.As(&texture_resource);

            return graphics::Texture(texture_resource, resource_view);
        }
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
        _token_store += _sections->on_item_selected += [&](const auto& item)
        {
            load_section(std::stoi(item.value(L"#")));
        };

        _texture_panel = _ui->find<Control>(Names::texture_panel);
        _texture = _ui->find<Image>(Names::texture);
    }

    void LauWindow::load_file(const std::string& filename)
    {
        try
        {
            std::vector<Listbox::Item> items;
            _drm = lau::load_drm(to_utf16(filename));
            for (const auto section : _drm->sections)
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

    void LauWindow::load_section(uint32_t index)
    {
        if (index < _drm->sections.size())
        {
            const auto& section = _drm->sections[index];
            if (section.header.type == lau::SectionType::Texture &&
                _drm->textures.find(section.header.id) != _drm->textures.end())
            {
                _texture_panel->set_visible(true);
                Microsoft::WRL::ComPtr<ID3D11Device> device;
                _device_window->context()->GetDevice(&device);
                _texture->set_texture(create_texture(device, _drm->textures[section.header.id]));
            }
            else
            {
                _texture_panel->set_visible(false);
            }
        }
    }
}
