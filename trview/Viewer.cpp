#include "stdafx.h"
#include "Viewer.h"
#include <trlevel/trlevel.h>

#include <string>
#include <algorithm>
#include <directxmath.h>

#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/Window.h>
#include <trview.ui/Label.h>

#include "RoomNavigator.h"
#include "CameraControls.h"
#include "Neighbours.h"
#include "TextureStorage.h"
#include "LevelInfo.h"

namespace trview
{
    Viewer::Viewer(Window window)
        : _window(window), _camera(window.width(), window.height()), _free_camera(window.width(), window.height())
    {
        _settings = load_user_settings();

        initialise_d3d();
        initialise_input();

        _texture_storage = std::make_unique<TextureStorage>(_device);
        _texture_storage->store("unknown", _texture_storage->coloured(0xff00ffff));
        _texture_storage->store("tomb3", _texture_storage->coloured(0xff00ff00));

        _font_factory = std::make_unique<ui::render::FontFactory>();

        generate_ui();
    }

    Viewer::~Viewer()
    {
        save_user_settings(_settings);
    }

    UserSettings Viewer::settings() const
    {
        return _settings;
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
        _control->set_handles_input(false);

        generate_tool_window();

        _texture_window = std::make_unique<TextureWindow>(_control.get());
        _texture_window->set_visible(false);

        _go_to_room = std::make_unique<GoToRoom>(_control.get());
        _go_to_room->room_selected += [&](uint32_t room)
        {
            select_room(room);
        };

        auto picking = std::make_unique<ui::Label>(ui::Point(500, 0), ui::Size(50, 30), ui::Colour(1, 0.5f, 0.5f, 0.5f), L"", 20.0f, ui::TextAlignment::Centre, ui::ParagraphAlignment::Centre);
        picking->set_visible(false);
        picking->set_handles_input(false);
        _picking = picking.get();
        _control->add_child(std::move(picking));

        _level_info = std::make_unique<LevelInfo>(*_control.get(), *_texture_storage.get());

        // Create the renderer for the UI based on the controls created.
        _ui_renderer = std::make_unique<ui::render::Renderer>(_device, _window.width(), _window.height());
        _ui_renderer->load(_control.get());
    }

    void Viewer::generate_tool_window()
    {
        using namespace ui;

        // This is the main tool window on the side of the screen.
        auto tool_window = std::make_unique<ui::StackPanel>(Point(), Size(150.0f, 310.0f), Colour(1.f, 0.5f, 0.5f, 0.5f), Size(5, 5));

        _room_navigator = std::make_unique<RoomNavigator>(*tool_window.get(), *_texture_storage.get());
        _room_navigator->on_room_selected += [&](uint32_t room) { select_room(room); };
        _room_navigator->on_highlight += [&](bool highlight) { toggle_highlight(); };

        _neighbours = std::make_unique<Neighbours>(*tool_window.get(), *_texture_storage.get());
        _neighbours->on_depth_changed += [&](int32_t value)
        {
            if (_level)
            {
                _level->set_neighbour_depth(value);
            }
        };
        _neighbours->on_enabled_changed += [&](bool enabled)
        {
            if (_level)
            {
                _level->set_highlight_mode(enabled ? Level::RoomHighlightMode::Neighbours : Level::RoomHighlightMode::None);
                _room_navigator->set_highlight(false);
            }
        };

        _camera_controls = std::make_unique<CameraControls>(*tool_window.get(), *_texture_storage.get());
        _camera_controls->on_reset += [&]() { _camera.reset(); };
        _camera_controls->on_mode_selected += [&](CameraMode mode) 
        {
            _camera_mode = mode;
            if (mode == CameraMode::Free)
            {
                _free_camera.set_position(_camera.position());
                _free_camera.set_rotation_yaw(_camera.rotation_yaw());
                _free_camera.set_rotation_pitch(_camera.rotation_pitch());
            }
        };
        _camera_controls->on_sensitivity_changed += [&](float value)
        {
            _camera_sensitivity = value;
            _settings.camera_sensitivity = value;
        };
        _camera_controls->set_sensitivity(_settings.camera_sensitivity);
        _camera_controls->set_mode(CameraMode::Orbit);

        _control->add_child(std::move(tool_window));
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
    }

    void Viewer::initialise_input()
    {
        _keyboard.register_key_up(std::bind(&Viewer::process_input_key, this, std::placeholders::_1));
        _keyboard.register_char(std::bind(&Viewer::process_char, this, std::placeholders::_1));

        _keyboard.register_key_down([&](uint16_t key)
        {
            switch (key)
            {
                case 'Q':
                {
                    _free_down = true;
                    break;
                }
                case 'E':
                {
                    _free_up = true;
                    break;
                }
                case 'W':
                {
                    _free_forward = true;
                    break;
                }
                case 'A':
                {
                    _free_left = true;
                    break;
                }
                case 'D':
                {
                    _free_right = true;
                    break;
                }
                case 'S':
                {
                    _free_backward = true;
                    break;
                }
            }
        });

        _keyboard.register_key_up([&](uint16_t key)
        {
            switch (key)
            {
                case 'Q':
                {
                    _free_down = false;
                    break;
                }
                case 'E':
                {
                    _free_up = false;
                    break;
                }
                case 'W':
                {
                    _free_forward = false;
                    break;
                }
                case 'A':
                {
                    _free_left = false;
                    break;
                }
                case 'D':
                {
                    _free_right = false;
                    break;
                }
                case 'S':
                {
                    _free_backward = false;
                    break;
                }
            }
        });

        using namespace input;
        _mouse.mouse_down += [&](Mouse::Button button)
        {
            if (button == Mouse::Button::Left)
            {
                if (!over_ui() && _picking->visible() && _current_pick.hit)
                {
                    select_room(_current_pick.room);
                    _camera_mode = CameraMode::Orbit;
                    _camera_controls->set_mode(CameraMode::Orbit);
                }
            }
            else if (button == Mouse::Button::Right)
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

            _control->mouse_up(client_cursor_position(_window));
        };

        _mouse.mouse_move += [&](long x, long y)
        {
            if (_rotating)
            {
                const float low_sensitivity = 200.0f;
                const float high_sensitivity = 25.0f;
                const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity) * _camera_sensitivity;

                if (_camera_mode == CameraMode::Orbit)
                {
                    const float yaw = _camera.rotation_yaw() + x / sensitivity;
                    const float pitch = _camera.rotation_pitch() + y / sensitivity;

                    _camera.set_rotation_yaw(yaw);
                    _camera.set_rotation_pitch(pitch);
                }
                else
                {
                    const float yaw = _free_camera.rotation_yaw() + x / sensitivity;
                    const float pitch = _free_camera.rotation_pitch() + y / sensitivity;

                    _free_camera.set_rotation_yaw(yaw);
                    _free_camera.set_rotation_pitch(pitch);
                }
            }

            _control->mouse_move(client_cursor_position(_window));
        };

        _mouse.mouse_wheel += [&](int16_t scroll)
        {
            _camera.set_zoom(_camera.zoom() + scroll / -100.0f);
        };

        // Add some extra handlers for the user interface. These will be merged in
        // to one at some point so that the UI can take priority where appropriate.
        _mouse.mouse_down += [&](Mouse::Button button)
        {
            // The client mouse coordinate is already relative to the root window (at present).
            _control->mouse_down(client_cursor_position(_window));
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
                case VK_F2:
                    toggle_texture_window();
                    break;
                case VK_RETURN:
                    toggle_highlight();
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
        if (_camera_mode == CameraMode::Free)
        {
            if (_free_left || _free_right || _free_forward || _free_backward || _free_up || _free_down)
            {
                DirectX::XMVECTOR movement = DirectX::XMVectorSet(
                    _free_left ? -1 : 0 + _free_right ? 1 : 0,
                    _free_up ? 1 : 0 + _free_down ? -1 : 0,
                    _free_forward ? 1 : 0 + _free_backward ? -1 : 0, 0);

                const float Speed = 20;
                _free_camera.move(DirectX::XMVectorScale(movement, _timer.elapsed() * Speed));
            }
        }
    }

    void Viewer::open(const std::wstring filename)
    {
        _settings.add_recent_file(filename);
        on_recent_files_changed(_settings.recent_files);
        save_user_settings(_settings);

        _current_level = trlevel::load_level(filename);
        _level = std::make_unique<Level>(_device, _current_level.get());

        // Set up the views.
        auto rooms = _level->room_info();
        _texture_window->set_textures(_level->level_textures());
        _camera.reset();

        // Reset UI buttons
        _room_navigator->set_max_rooms(rooms.size());
        _room_navigator->set_highlight(false);
        select_room(0);

        _neighbours->set_enabled(false);

        // Strip the last part of the path away.
        auto last_index = std::min(filename.find_last_of('\\'), filename.find_last_of('/'));
        auto name = last_index == filename.npos ? filename : filename.substr(std::min(last_index + 1, filename.size()));
        _level_info->set_level(name);
        _level_info->set_level_version(_current_level->get_version());
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

        pick();

        _context->OMSetRenderTargets(1, &_render_target_view.p, _depth_stencil_view);
        _context->OMSetBlendState(_blend_state, 0, 0xffffffff);

        float colours[4] = { 0.f, 0.2f, 0.4f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);
        _context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

        render_scene();

        _context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
        render_ui();

        _swap_chain->Present(1, 0);
    }

    // Determines whether the cursor is over a UI element that would take any input.
    // Returns: True if there is any UI under the cursor that would take input.
    bool Viewer::over_ui() const
    {
        return _control->is_mouse_over(client_cursor_position(_window));
    }

    void Viewer::pick()
    {
        if (!_level || window_is_minimised(_window) || over_ui() || cursor_outside_window(_window))
        {
            _picking->set_visible(false);
            return;
        }

        using namespace DirectX;

        auto position = _camera_mode == CameraMode::Free ? _free_camera.position() : _camera.position();
        auto world = XMMatrixTranslationFromVector(position);
        auto projection = _camera_mode == CameraMode::Free ? _free_camera.projection() : _camera.projection();
        auto view = _camera_mode == CameraMode::Free ? _free_camera.view() : _camera.view();

        ui::Point mouse_pos = client_cursor_position(_window);
        auto direction = XMVector3Normalize(XMVector3Unproject(
            XMVectorSet(mouse_pos.x, mouse_pos.y, 1, 0), 0, 0, _window.width(), _window.height(), 0, 1.0f, projection, view, world));

        auto result = _level->pick(position, direction);

        _picking->set_visible(result.hit);
        if (result.hit)
        {
            XMFLOAT3 screen_pos;
            XMStoreFloat3(&screen_pos, XMVector3Project(result.position, 0, 0, _window.width(), _window.height(), 0, 1.0f, projection, view, XMMatrixIdentity()));
            _picking->set_position(ui::Point(screen_pos.x - _picking->size().width, screen_pos.y - _picking->size().height));
            _picking->set_text(std::to_wstring(result.room));
        }
        _current_pick = result;
    }

    void Viewer::render_scene()
    {
        _context->OMSetDepthStencilState(_depth_stencil_state, 1);
        if (_level)
        {
            using namespace DirectX;

            // Update the view matrix based on the room selected in the room window.
            auto room = _current_level->get_room(_level->selected_room());

            XMVECTOR target_position = XMVectorSet(
                (room.info.x / 1024.f) + room.num_x_sectors / 2.f,
                (room.info.yBottom / -1024.f) + (room.info.yTop - room.info.yBottom) / -1024.f / 2.0f,
                (room.info.z / 1024.f) + room.num_z_sectors / 2.f, 0);

            _camera.set_target(target_position);
            
            auto view_projection = _camera_mode == CameraMode::Orbit ? _camera.view_projection() : _free_camera.view_projection();

            _level->render(_context, view_projection);
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

    void Viewer::cycle_back()
    {
        _texture_window->cycle_back();
    }

    void Viewer::toggle_texture_window()
    {
        _texture_window->toggle_visibility();
    }

    void Viewer::toggle_highlight()
    {
        if (_level)
        {
            if (_level->highlight_mode() == Level::RoomHighlightMode::Highlight)
            {
                _level->set_highlight_mode(Level::RoomHighlightMode::None);
                _room_navigator->set_highlight(false);
                _neighbours->set_enabled(false);
            }
            else
            {
                _level->set_highlight_mode(Level::RoomHighlightMode::Highlight);
                _room_navigator->set_highlight(true);
                _neighbours->set_enabled(false);
            }
        }
    }

    void Viewer::select_room(uint32_t room)
    {
        if (_current_level && room < _current_level->num_rooms())
        {
            _level->set_selected_room(room);

            _room_navigator->set_selected_room(room);
            _room_navigator->set_room_info(_level->room_info(room));
        }
    }
}
