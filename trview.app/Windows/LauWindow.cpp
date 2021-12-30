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

        DirectX::DDS_PIXELFORMAT dds_format(const lau::Texture& texture)
        {
            using namespace DirectX;
            if (texture.format == "DXT1") { return DDSPF_DXT1; }
            if (texture.format == "DXT2") { return DDSPF_DXT2; }
            if (texture.format == "DXT3") { return DDSPF_DXT3; }
            if (texture.format == "DXT4") { return DDSPF_DXT4; }
            if (texture.format == "DXT5") { return DDSPF_DXT5; }
            return DDSPF_A8R8G8B8;
        }

        graphics::Texture create_texture(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const lau::Texture& texture)
        {
            using namespace Microsoft::WRL;
            using namespace DirectX;

            DDS_HEADER header;
            memset(&header, 0, sizeof(header));
            header.size = sizeof(header);
            header.flags = 0x1 | 0x2 | 0x4 | 0x1000 | 0x80000;
            header.height = texture.height;
            header.width = texture.width;
            header.pitchOrLinearSize = texture.data.size();
            header.ddspf = dds_format(texture);
            header.caps = 4096;

            std::vector<uint8_t> final_data(4 + sizeof(header) + texture.data.size(), 0u);
            uint32_t magic = 542327876u;
            memcpy(&final_data[0], &magic, sizeof(magic));
            memcpy(&final_data[4], &header, sizeof(header));
            memcpy(&final_data[4 + sizeof(header)], &texture.data[0], texture.data.size());

            ComPtr<ID3D11Resource> resource;
            ComPtr<ID3D11ShaderResourceView> resource_view;
            HRESULT hr = DirectX::CreateDDSTextureFromMemory(device.Get(), &final_data[0], final_data.size(), resource.GetAddressOf(), resource_view.GetAddressOf());

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
        const std::shared_ptr<lau::IDrmLoader>& drm_loader,
        const trview::Window& parent,
        const std::shared_ptr<ui::ILoader>& loader)
        : CollapsiblePanel(device_window_source, renderer_source(Size(600, 600)), parent, L"trview.lau", L"LAU Development", input_source,
             Size(600, 600), loader->load_from_resource(IDR_UI_LAU_WINDOW)), _dialogs(dialogs), _drm_loader(drm_loader)
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
        _texture_stats = _ui->find<Listbox>(Names::texture_stats);
        _filename = _ui->find<Label>(Names::filename);
    }

    void LauWindow::load_file(const std::string& filename)
    {
        try
        {
            _drm = _drm_loader->load(to_utf16(filename));
            std::vector<Listbox::Item> items;
            for (const auto section : _drm->sections)
            {
                items.push_back({{ { L"#", std::to_wstring(section.index) },
                                   { L"ID", std::to_wstring(section.header.id) },
                                   { L"Size", std::to_wstring(section.data.size()) },
                                   { L"Type", section_type_to_string(section.header.type) } }});
            }
            _sections->clear_selection();
            _sections->set_items(items);
            _texture_panel->set_visible(false);

            auto last_index = std::min(filename.find_last_of('\\'), filename.find_last_of('/'));
            auto name = last_index == filename.npos ? filename : filename.substr(std::min(last_index + 1, filename.size()));
            _filename->set_text(to_utf16(name));
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
                const auto& texture = _drm->textures[section.header.id];

                _texture_panel->set_visible(true);
                Microsoft::WRL::ComPtr<ID3D11Device> device;
                _device_window->context()->GetDevice(&device);
                _texture->set_texture(create_texture(device, texture));

                std::vector<Listbox::Item> stats;
                stats.push_back(make_item(L"ID", std::to_wstring(texture.id)));
                stats.push_back(make_item(L"Width", std::to_wstring(texture.width)));
                stats.push_back(make_item(L"Height", std::to_wstring(texture.height)));
                stats.push_back(make_item(L"Format", to_utf16(texture.format)));
                _texture_stats->set_items(stats);
            }
            else
            {
                _texture_panel->set_visible(false);
            }
        }
    }
}
