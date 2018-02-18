#include "stdafx.h"
#include "Viewer.h"
#include <trlevel/trlevel.h>

#include <trview.common/FileLoader.h>

#include <vector>
#include <string>
#include <sstream>
#include <d3dcompiler.h>
#include <directxmath.h>

#include <trview.ui/Window.h>
#include <trview.ui/Label.h>
#include <trview.ui/Button.h>
#include <trview.ui/Slider.h>
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/GroupBox.h>

namespace trview
{
    Viewer::Viewer(Window window)
        : _window(window), _camera(window.width(), window.height())
    {
        initialise_d3d();
        initialise_input();

        _font_factory = std::make_unique<ui::render::FontFactory>();

        generate_ui();
    }

    void Viewer::generate_ui()
    {
        // Create the user interface window. At the moment this is going to be a bar on the side, 
        // but this can change over time. For now make a really boring gray window.
        _control =
            std::make_unique<ui::Window>(
                ui::Point(0, 0),
                ui::Size(_window.width(), _window.height()),
                ui::Colour(0.f, 0.f, 0.f, 0.f));

        generate_tool_window();

        _room_window = std::make_unique<RoomWindow>(_control.get());
        _texture_window = std::make_unique<TextureWindow>(_control.get());

        _go_to_room = std::make_unique<GoToRoom>(_control.get());
        _go_to_room->room_selected += [&](uint32_t room)
        {
            if (_current_level && room < _current_level->num_rooms())
            {
                _room_window->select_room(room);
                regenerate_neighbours();
            }
        };

        // Create the renderer for the UI based on the controls created.
        _ui_renderer = std::make_unique<ui::render::Renderer>(_device, _window.width(), _window.height());
        _ui_renderer->load(_control.get());
    }

    void Viewer::generate_tool_window()
    {
        using namespace ui;

        // This is the main tool window on the side of the screen.
        auto tool_window = std::make_unique<ui::Window>(
            Point(0, 0),
            Size(150.0f, 175.0f),
            Colour(1.f, 0.5f, 0.5f, 0.5f));

        tool_window->add_child(generate_room_window(Point(5, 5)));
        tool_window->add_child(generate_neighbours_window(Point(5,60)));
        tool_window->add_child(generate_camera_window(Point(5, 120)));
        _control->add_child(std::move(tool_window));
    }

    std::unique_ptr<ui::Window> Viewer::generate_neighbours_window(ui::Point point)
    {
        using namespace ui;

        auto neighbours_group = std::make_unique<GroupBox>(
            point,
            Size(140, 50),
            Colour(1.0f, 0.5f, 0.5f, 0.5f),
            Colour(1.0f, 0.0f, 0.0f, 0.0f),
            L"Neighbours");

        auto room_neighbours = std::make_unique<Button>(
            Point(12, 20),
            Size(16, 16),
            create_coloured_texture(0xff0000ff),
            create_coloured_texture(0xff00ff00));
        room_neighbours->on_click += [&]() { _room_neighbours = !_room_neighbours; };

        auto neighbours_depth_label = std::make_unique<Label>(
            Point(40, 20),
            Size(40, 20),
            Colour(1.0f, 0.5f, 0.5f, 0.5f),
            L"Depth",
            10.f);

        auto neighbours_depth = std::make_unique<NumericUpDown>(
            Point(90, 16),
            Size(40, 20),
            Colour(1.0f, 0.4f, 0.4f, 0.4f),
            create_coloured_texture(0xff0000ff),
            create_coloured_texture(0xff00ff00),
            0,
            10);
        neighbours_depth->set_value(1);

        neighbours_depth->on_value_changed += [&](int value) 
        { 
            _neighbour_depth = value; 
            regenerate_neighbours();
        };

        neighbours_group->add_child(std::move(room_neighbours));
        neighbours_group->add_child(std::move(neighbours_depth_label));
        neighbours_group->add_child(std::move(neighbours_depth));
        return neighbours_group;
    }

    std::unique_ptr<ui::Window> Viewer::generate_room_window(ui::Point point)
    {
        using namespace ui;

        auto rooms_groups = std::make_unique<GroupBox>(
            point,
            Size(140, 50),
            Colour(1.0f, 0.5f, 0.5f, 0.5f),
            Colour(1.0f, 0.0f, 0.0f, 0.0f),
            L"Rooms");

        auto room_highlight = std::make_unique<Button>(
            Point(12, 20),
            Size(16, 16),
            create_coloured_texture(0xff0000ff),
            create_coloured_texture(0xff00ff00));

        auto room_highlight_label = std::make_unique<Label>(
            Point(40, 20),
            Size(40, 20),
            Colour(1.0f, 0.5f, 0.5f, 0.5f),
            L"Highlight",
            10.f);

        room_highlight->on_click += [&]() { toggle_highlight(); };
        _room_highlight = room_highlight.get();

        rooms_groups->add_child(std::move(room_highlight));
        rooms_groups->add_child(std::move(room_highlight_label));

        return rooms_groups;
    }

    std::unique_ptr<ui::Window> Viewer::generate_camera_window(ui::Point point)
    {
        using namespace ui;

        auto camera_window = std::make_unique<GroupBox>(
            point,
            Size(140, 50),
            Colour(1.0f, 0.5f, 0.5f, 0.5f),
            Colour(1.0f, 0.0f, 0.0f, 0.0f),
            L"Camera");

        // Camera section for the menu bar.
        auto camera_sensitivity = std::make_unique<ui::Slider>(Point(12, 20), Size(120, 20));
        
        camera_sensitivity->on_value_changed += [&](float value)
        {
            _camera_sensitivity = value;
        };

        camera_window->add_child(std::move(camera_sensitivity));
        return camera_window;
    }

    // Temporary function to createa a 50x50 coloured rectangle.
    Texture Viewer::create_coloured_texture(uint32_t colour)
    {
        std::vector<uint32_t> data(50 * 50, colour);
        D3D11_SUBRESOURCE_DATA srd;
        memset(&srd, 0, sizeof(srd));
        srd.pSysMem = &data[0];
        srd.SysMemPitch = sizeof(uint32_t) * 50;

        D3D11_TEXTURE2D_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.Width = 50;
        desc.Height = 50;
        desc.MipLevels = desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        Texture tex;
        _device->CreateTexture2D(&desc, &srd, &tex.texture);
        _device->CreateShaderResourceView(tex.texture, nullptr, &tex.view);
        return tex;
    }

    void Viewer::initialise_d3d()
    {
        // Swap chain description.
        DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
        swap_chain_desc.BufferCount = 1;
        swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.BufferDesc.Height = _window.height();
        swap_chain_desc.BufferDesc.Width = _window.width();
        swap_chain_desc.BufferDesc.RefreshRate.Numerator = 1;
        swap_chain_desc.BufferDesc.RefreshRate.Denominator = 60;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.OutputWindow = _window.window();
        swap_chain_desc.Windowed = TRUE;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &_swap_chain,
            &_device,
            nullptr,
            &_context);

        CComPtr<ID3D11Texture2D> back_buffer;

        _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
        _device->CreateRenderTargetView(back_buffer, nullptr, &_render_target_view);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<float>(_window.width());
        viewport.Height = static_cast<float>(_window.height());
        viewport.MaxDepth = 1;
        viewport.MinDepth = 0;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        _context->RSSetViewports(1, &viewport);

        D3D11_BLEND_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        _device->CreateBlendState(&desc, &_blend_state);

        // Initialize the description of the depth buffer.
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        memset(&depthBufferDesc, 0, sizeof(depthBufferDesc));

        // Set up the description of the depth buffer.
        depthBufferDesc.Width = _window.width();
        depthBufferDesc.Height = _window.height();
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        _device->CreateTexture2D(&depthBufferDesc, NULL, &_depth_stencil_buffer);

        // Initialize the description of the stencil state.
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));

        // Set up the description of the stencil state.
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing.
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing.
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        _device->CreateDepthStencilState(&depthStencilDesc, &_depth_stencil_state);

        _context->OMSetDepthStencilState(_depth_stencil_state, 1);

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));

        // Set up the depth stencil view description.
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // Create the depth stencil view.
        _device->CreateDepthStencilView(_depth_stencil_buffer, &depthStencilViewDesc, &_depth_stencil_view);

        std::vector<char> vs_data = load_file(L"level_vertex_shader.cso");

        D3D11_INPUT_ELEMENT_DESC input_desc[3];
        memset(&input_desc, 0, sizeof(input_desc));
        input_desc[0].SemanticName = "Position";
        input_desc[0].SemanticIndex = 0;
        input_desc[0].InstanceDataStepRate = 0;
        input_desc[0].InputSlot = 0;
        input_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

        input_desc[1].SemanticName = "Texcoord";
        input_desc[1].SemanticIndex = 0;
        input_desc[1].InstanceDataStepRate = 0;
        input_desc[1].InputSlot = 0;
        input_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;

        input_desc[2].SemanticName = "Texcoord";
        input_desc[2].SemanticIndex = 1;
        input_desc[2].InstanceDataStepRate = 0;
        input_desc[2].InputSlot = 0;
        input_desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        hr = _device->CreateInputLayout(input_desc, 3, &vs_data[0], vs_data.size(), &_input_layout);

        hr = _device->CreateVertexShader(&vs_data[0], vs_data.size(), nullptr, &_vertex_shader);

        std::vector<char> ps_data = load_file(L"level_pixel_shader.cso");
        hr = _device->CreatePixelShader(&ps_data[0], ps_data.size(), nullptr, &_pixel_shader);

        // Create a texture sampler state description.
        D3D11_SAMPLER_DESC sampler_desc;
        memset(&desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the texture sampler state.
        _device->CreateSamplerState(&sampler_desc, &_sampler_state);
    }

    void Viewer::initialise_input()
    {
        _keyboard.register_key_up(std::bind(&Viewer::process_input_key, this, std::placeholders::_1));
        _keyboard.register_char(std::bind(&Viewer::process_char, this, std::placeholders::_1));

        using namespace input;
        _mouse.mouse_down += [&](Mouse::Button button)
        {
            if (button == Mouse::Button::Right)
            {
                _rotating = true;
            }
        };

        _mouse.mouse_up += [&](Mouse::Button button)
        {
            if (button == Mouse::Button::Right)
            {
                _rotating = false;
            }
            
            POINT cursor_pos;
            GetCursorPos(&cursor_pos);
            ScreenToClient(_window.window(), &cursor_pos);

            _control->mouse_up(ui::Point(cursor_pos.x, cursor_pos.y));
        };

        _mouse.mouse_move += [&](long x, long y)
        {
            if (_rotating)
            {
                const float low_sensitivity = 200.0f;
                const float high_sensitivity = 25.0f;
                const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity) * _camera_sensitivity;
                _camera.set_rotation_yaw(_camera.rotation_yaw() + x / sensitivity);
                _camera.set_rotation_pitch(_camera.rotation_pitch() + y / sensitivity);
            }

            POINT cursor_pos;
            GetCursorPos(&cursor_pos);
            ScreenToClient(_window.window(), &cursor_pos);

            _control->mouse_move(ui::Point(cursor_pos.x, cursor_pos.y));
        };

        _mouse.mouse_wheel += [&](int16_t scroll)
        {
            _camera.set_zoom(_camera.zoom() + scroll / -100.0f);
        };

        // Add some extra handlers for the user interface. These will be merged in
        // to one at some point so that the UI can take priority where appropriate.
        _mouse.mouse_down += [&](Mouse::Button button)
        {
            POINT cursor_pos;
            GetCursorPos(&cursor_pos);
            ScreenToClient(_window.window(), &cursor_pos);

            // The client mouse coordinate is already relative to the root windot (at
            // present).
            _control->mouse_down(ui::Point(cursor_pos.x, cursor_pos.y));
        };
    }

    void Viewer::process_input_key(uint16_t key)
    {
        if (_go_to_room->visible())
        {
            if (key == 'R' && _keyboard.control())
            {
                _go_to_room->toggle_visible();
            }
            else
            {
                _go_to_room->input(key);
            }
        }
        else
        {
            switch (key)
            {
            case 'R':
                if (_keyboard.control())
                {
                    _go_to_room->toggle_visible();
                }
                break;
            case VK_PRIOR:
                cycle_back();
                break;
            case VK_NEXT:
                cycle();
                break;
            case VK_HOME:
                cycle_room_back();
                break;
            case VK_END:
                cycle_room();
                break;
            case VK_F1:
                toggle_room_window();
                break;
            case VK_F2:
                toggle_texture_window();
                break;
            case VK_RETURN:
                toggle_highlight();
                _room_highlight->set_state(_highlight);
                break;
            case VK_INSERT:
            {
                // Reset the camera to defaults.
                _camera.set_rotation_yaw(0.f);
                _camera.set_rotation_pitch(0.78539f);
                _camera.set_zoom(8.f);
                break;
            }
            }
        }
    }

    void Viewer::process_char(uint16_t character)
    {
        if (_go_to_room->visible())
        {
            _go_to_room->character(character);
        }
    }

    void Viewer::update_camera()
    {
    }

    void Viewer::generate_textures()
    {
        _level_textures.clear();

        // Load the textures from the level and then allow to cycle through them?
        for (uint32_t i = 0; i < _current_level->num_textiles(); ++i)
        {
            auto t16 = _current_level->get_textile16(i);

            std::vector<uint32_t> data(256 * 256, 0u);

            uint32_t index = 0;
            for (auto t : t16.Tile)
            {
                data[index++] = trlevel::convert_textile16(t);
            }

            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &data[0];
            srd.SysMemPitch = sizeof(uint32_t) * 256;

            D3D11_TEXTURE2D_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Width = 256;
            desc.Height = 256;
            desc.MipLevels = desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            Texture tex;
            _device->CreateTexture2D(&desc, &srd, &tex.texture);
            _device->CreateShaderResourceView(tex.texture, nullptr, &tex.view);
            _level_textures.push_back(tex);
        }
    }

    void Viewer::generate_rooms()
    {
        const uint16_t num_rooms = _current_level->num_rooms();
        for (uint16_t i = 0; i < num_rooms; ++i)
        {
            auto room = _current_level->get_room(i);

            // Convert that room into a Room that we can use in the UI.
            _level_rooms.push_back(std::make_unique<Room>(_device, *_current_level, room));
        }
    }

    void Viewer::open(const std::wstring filename)
    {
        _level_textures.clear();
        _level_rooms.clear();
        _current_level = trlevel::load_level(filename);

        generate_textures();

        generate_rooms();

        // Set up the views.
        _texture_window->set_textures(_level_textures);

        std::vector<RoomInfo> room_infos;
        for (const auto& r : _level_rooms)
        {
            room_infos.push_back(r->info());
        }
        _room_window->set_rooms(room_infos);
        regenerate_neighbours();
    }

    void Viewer::on_char(uint16_t character)
    {
        _keyboard.set_control(GetKeyState(VK_CONTROL));
        _keyboard.on_char(character);
    }

    void Viewer::on_key_down(uint16_t key)
    {
        _keyboard.set_control(GetKeyState(VK_CONTROL));
        _keyboard.on_key_down(key);
    }

    void Viewer::on_key_up(uint16_t key)
    {
        _keyboard.set_control(GetKeyState(VK_CONTROL));
        _keyboard.on_key_up(key);
    }

    void Viewer::on_input(const RAWINPUT& input)
    {
        _mouse.process_input(input);
    }

    void Viewer::render()
    {
        _timer.update();

        update_camera();

        _context->OMSetRenderTargets(1, &_render_target_view.p, _depth_stencil_view);
        _context->OMSetBlendState(_blend_state, 0, 0xffffffff);

        float colours[4] = { 0.f, 0.2f, 0.4f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);
        _context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

        if (_level_textures.size())
        {
            render_scene();
        }

        _context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
        render_ui();

        _swap_chain->Present(1, 0);
    }

    void Viewer::render_scene()
    {
        _context->OMSetDepthStencilState(_depth_stencil_state, 1);

        using namespace DirectX;

        // Update the view matrix based on the room selected in the room window.
        auto room = _current_level->get_room(_room_window->selected_room());

        XMVECTOR target_position = XMVectorSet(
            (room.info.x / 1024.f) + room.num_x_sectors / 2.f, 
            (room.info.yBottom / -1024.f) + (room.info.yTop - room.info.yBottom) / -1024.f / 2.0f,
            (room.info.z / 1024.f) + room.num_z_sectors / 2.f, 0);

        _camera.set_target(target_position);

        auto view_projection = _camera.view_projection();

        _context->PSSetSamplers(0, 1, &_sampler_state.p);
        _context->IASetInputLayout(_input_layout);
        _context->VSSetShader(_vertex_shader, nullptr, 0);
        _context->PSSetShader(_pixel_shader, nullptr, 0);
        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        const uint16_t selected_room = _room_window->selected_room();

        if (_room_neighbours)
        {
            for (uint16_t i : _neighbours)
            {
                _level_rooms[i]->render(_context, view_projection, _level_textures, i == selected_room ? Room::SelectionMode::Selected : Room::SelectionMode::Neighbour);
            }
        }
        else
        {
            for (std::size_t i = 0; i < _level_rooms.size(); ++i)
            {
                _level_rooms[i]->render(_context, view_projection, _level_textures, (_highlight && selected_room == i) ? Room::SelectionMode::Selected : _highlight ? Room::SelectionMode::NotSelected : Room::SelectionMode::Selected);
            }
        }
    }

    void Viewer::regenerate_neighbours()
    {
        const uint16_t selected_room = _room_window->selected_room();
        _neighbours = std::set<uint16_t>{ selected_room };
        if (_current_level && selected_room < _current_level->num_rooms())
        {
            generate_neighbours(_neighbours, selected_room, 1, _neighbour_depth);
        }
    }

    void Viewer::generate_neighbours(std::set<uint16_t>& all_rooms, uint16_t selected_room, int32_t current_depth, int32_t max_depth)
    {
        if (current_depth > max_depth)
        {
            return;
        }

        const auto neighbours = _level_rooms[selected_room]->neighbours();
        for (auto room = neighbours.begin(); room != neighbours.end(); ++room)
        {
            all_rooms.insert(*room);
            generate_neighbours(all_rooms, *room, current_depth + 1, max_depth);
        }
    }

    void Viewer::render_ui()
    {
        _ui_renderer->render(_context);
    }

    void Viewer::cycle()
    {
        _texture_window->cycle();
    }

    void Viewer::cycle_room()
    {
        _room_window->cycle();
        regenerate_neighbours();
    }

    void Viewer::cycle_back()
    {
        _texture_window->cycle_back();
    }

    void Viewer::cycle_room_back()
    {
        _room_window->cycle_back();
        regenerate_neighbours();
    }

    void Viewer::toggle_room_window()
    {
        _room_window->toggle_visibility();
    }

    void Viewer::toggle_texture_window()
    {
        _texture_window->toggle_visibility();
    }

    void Viewer::toggle_highlight()
    {
        _highlight = !_highlight;
    }
}
