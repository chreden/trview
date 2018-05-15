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

#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/RenderTarget.h>

#include "RoomNavigator.h"
#include "CameraControls.h"
#include "Neighbours.h"
#include "TextureStorage.h"
#include "LevelInfo.h"
#include "DefaultTextures.h"
#include "DefaultShaders.h"

namespace trview
{
    Viewer::Viewer(Window window)
        : _window(window), _camera(window.width(), window.height()), _free_camera(window.width(), window.height())
    {
        _settings = load_user_settings();

        initialise_d3d();
        initialise_input();

        _texture_storage = std::make_unique<TextureStorage>(_device);
        load_default_textures(_device, *_texture_storage.get());

        _shader_storage = std::make_unique<graphics::ShaderStorage>();
        load_default_shaders(_device, *_shader_storage.get());

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
        _control = std::make_unique<ui::Window>(ui::Point(), Size(static_cast<float>(_window.width()), static_cast<float>(_window.height())), ui::Colour(0.f, 0.f, 0.f, 0.f)); 
        _control->set_handles_input(false);

        generate_tool_window();

        _texture_window = std::make_unique<TextureWindow>(_control.get());
        _texture_window->set_visible(false);

        _go_to_room = std::make_unique<GoToRoom>(_control.get());
        _go_to_room->room_selected += [&](uint32_t room)
        {
            select_room(room);
            set_camera_mode(CameraMode::Orbit);
        };

        auto picking = std::make_unique<ui::Label>(ui::Point(500, 0), Size(50, 30), ui::Colour(1, 0.5f, 0.5f, 0.5f), L"", 20.0f, ui::TextAlignment::Centre, ui::ParagraphAlignment::Centre);
        picking->set_visible(false);
        picking->set_handles_input(false);
        _picking = picking.get();
        _control->add_child(std::move(picking));

        _level_info = std::make_unique<LevelInfo>(*_control.get(), *_texture_storage.get());

        // Create the renderer for the UI based on the controls created.
        _ui_renderer = std::make_unique<ui::render::Renderer>(_device, *_shader_storage.get(), _window.width(), _window.height());
        _ui_renderer->load(_control.get());

        _map_renderer = std::make_unique<ui::render::MapRenderer>(_device, *_shader_storage.get(), _window.width(), _window.height());
        
    }

    void Viewer::generate_tool_window()
    {
        using namespace ui;

        // This is the main tool window on the side of the screen.
        auto tool_window = std::make_unique<ui::StackPanel>(Point(), Size(150.0f, 348.0f), Colour(1.f, 0.5f, 0.5f, 0.5f), Size(5, 5));

        _room_navigator = std::make_unique<RoomNavigator>(*tool_window.get(), *_texture_storage.get());
        _room_navigator->on_room_selected += [&](uint32_t room) { select_room(room); };
        _room_navigator->on_highlight += [&](bool) { toggle_highlight(); };
        _room_navigator->on_flip += [&](bool flip) { set_alternate_mode(flip); };

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
        _camera_controls->on_mode_selected += [&](CameraMode mode) { set_camera_mode(mode); };
        _camera_controls->on_sensitivity_changed += [&](float value)
        {
            _camera_sensitivity = value;
            _settings.camera_sensitivity = value;
        };

        _camera_controls->on_movement_speed_changed += [&](float value)
        {
            _camera_movement_speed = value; 
            _settings.camera_movement_speed = value;
        };

        _camera_controls->set_sensitivity(_settings.camera_sensitivity);
        _camera_controls->set_mode(CameraMode::Orbit);

        _camera_controls->set_movement_speed (
            _settings.camera_movement_speed == 0? _CAMERA_MOVEMENT_SPEED_DEFAULT: _settings.camera_movement_speed
        );

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

        D3D11CreateDeviceAndSwapChain(
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

        create_render_target();

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

        _context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);
    }

    void Viewer::initialise_input()
    {
        _keyboard.register_key_up(std::bind(&Viewer::process_input_key, this, std::placeholders::_1));
        _keyboard.register_char(std::bind(&Viewer::process_char, this, std::placeholders::_1));

        _keyboard.register_key_down([&](auto key) {_camera_input.key_down(key); });
        _keyboard.register_key_up([&](auto key) {_camera_input.key_up(key); });

        _keyboard.register_key_down([&](uint16_t key)
        {
            switch (key)
            {
                case 'P':
                {
                    if (_level)
                    {
                        set_alternate_mode(!_level->alternate_mode());
                    }
                    break;
                }
            }
        });

        setup_camera_input();

        using namespace input;

        _mouse.mouse_down += [&](Mouse::Button button)
        {
            if (button == Mouse::Button::Left)
            {
                if (!over_ui() && !over_map() && _picking->visible() && _current_pick.hit)
                {
                    select_room(_current_pick.room);
                    set_camera_mode(CameraMode::Orbit);
                }
                else if (over_map())
                {
                    std::shared_ptr<Sector> sector = _map_renderer->sector_at_cursor(); 
                    if (sector != nullptr)
                    {
                        if (sector->flags & SectorFlag::Portal)
                            select_room(sector->portal());
                        else if (sector->flags & SectorFlag::RoomBelow)
                            select_room(sector->room_below()); 
                    }
                }
            }
            else if (button == Mouse::Button::Right)
            {
                if (over_map())
                {
                    std::shared_ptr<Sector> sector = _map_renderer->sector_at_cursor(); 
                    if (sector != nullptr && (sector->flags & SectorFlag::RoomAbove))
                        select_room(sector->room_above()); 
                }
            }
        };

        _mouse.mouse_up += [&](auto) { _control->mouse_up(client_cursor_position(_window)); };
        _mouse.mouse_move += [&](auto, auto) { _control->mouse_move(client_cursor_position(_window)); };

        // Add some extra handlers for the user interface. These will be merged in
        // to one at some point so that the UI can take priority where appropriate.
        _mouse.mouse_down += [&](Mouse::Button)
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
                {
                    if (_keyboard.control())
                    {
                        _go_to_room->toggle_visible();
                    }
                    break;
                }
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
        if (_camera_mode == CameraMode::Free || _camera_mode == CameraMode::Axis)
        {
            const float Speed = std::max(0.01f, _camera_movement_speed) * _CAMERA_MOVEMENT_SPEED_MULTIPLIER;
            _free_camera.move(_camera_input.movement() * _timer.elapsed() * Speed);

            if (_level)
            {
                _level->on_camera_moved();
            }
        }
    }

    void Viewer::open(const std::wstring filename)
    {
        try
        {
            _current_level = trlevel::load_level(filename);
        }
        catch(...)
        {
            MessageBox(_window.window(), L"Failed to load level", L"Error", MB_OK);
            return;
        }

        on_file_loaded(filename);
        _settings.add_recent_file(filename);
        on_recent_files_changed(_settings.recent_files);
        save_user_settings(_settings);

        _level = std::make_unique<Level>(_device, *_shader_storage.get(), _current_level.get());
        _level->on_room_selected += [&](uint16_t room) { select_room(room); };
        _level->on_alternate_mode_selected += [&](bool enabled) { set_alternate_mode(enabled); };

        // Set up the views.
        auto rooms = _level->room_info();
        _texture_window->set_textures(_level->level_textures());
        _camera.reset();

        // Reset UI buttons
        _room_navigator->set_max_rooms(static_cast<uint32_t>(rooms.size()));
        _room_navigator->set_highlight(false);
        _room_navigator->set_flip(false);
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
        _keyboard.on_char(character);
    }

    void Viewer::on_key_down(uint16_t key)
    {
        _keyboard.on_key_down(key);
    }

    void Viewer::on_key_up(uint16_t key)
    {
        _keyboard.on_key_up(key);
    }

    void Viewer::on_input(const RAWINPUT& input)
    {
        _mouse.process_input(input);
    }

    void Viewer::on_scroll(int16_t delta)
    {
        _mouse.process_scroll(delta);
    }

    void Viewer::render()
    {
        _timer.update();

        update_camera();

        pick();

        _render_target->apply(_context);
        _context->OMSetBlendState(_blend_state.Get(), 0, 0xffffffff);
        _render_target->clear(_context, DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));

        render_scene();

        _context->OMSetBlendState(_blend_state.Get(), 0, 0xffffffff);
        render_ui();

        render_map();

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
        if (!_level || window_is_minimised(_window) || over_ui() || over_map() || cursor_outside_window(_window))
        {
            _picking->set_visible(false);
            return;
        }

        using namespace DirectX;
        using namespace SimpleMath;

        const ICamera& camera = current_camera();
        const Vector3 position = camera.position();
        auto world = Matrix::CreateTranslation(position);
        auto projection = camera.projection();
        auto view = camera.view();

        ui::Point mouse_pos = client_cursor_position(_window);

        Vector3 direction = XMVector3Unproject(Vector3(mouse_pos.x, mouse_pos.y, 1), 0, 0, static_cast<float>(_window.width()), static_cast<float>(_window.height()), 0, 1.0f, projection, view, world);
        direction.Normalize();

        auto result = _level->pick(position, direction);

        _picking->set_visible(result.hit);
        if (result.hit)
        {
            Vector3 screen_pos = XMVector3Project(result.position, 0, 0, static_cast<float>(_window.width()), static_cast<float>(_window.height()), 0, 1.0f, projection, view, XMMatrixIdentity());
            _picking->set_position(ui::Point(screen_pos.x - _picking->size().width, screen_pos.y - _picking->size().height));
            _picking->set_text(std::to_wstring(result.room));
        }
        _current_pick = result;
    }

    void Viewer::render_scene()
    {
        _context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);
        if (_level)
        {
            // Update the view matrix based on the room selected in the room window.
            if (_current_level->num_rooms() > 0)
            {
                auto room = _current_level->get_room(_level->selected_room());

                DirectX::SimpleMath::Vector3 target_position(
                    (room.info.x / 1024.f) + room.num_x_sectors / 2.f,
                    (room.info.yBottom / -1024.f) + (room.info.yTop - room.info.yBottom) / -1024.f / 2.0f,
                    (room.info.z / 1024.f) + room.num_z_sectors / 2.f);

                _camera.set_target(target_position);
            }
            _level->render(_context, current_camera());
        }
    }

    const ICamera& Viewer::current_camera() const
    {
        if (_camera_mode == CameraMode::Orbit)
        {
            return _camera;
        }
        return _free_camera;
    }

    ICamera& Viewer::current_camera()
    {
        if (_camera_mode == CameraMode::Orbit)
        {
            return _camera;
        }
        return _free_camera;
    }

    void Viewer::set_camera_mode(CameraMode camera_mode)
    {
        if (_camera_mode == camera_mode) 
        {
            return;
        }

        if (camera_mode == CameraMode::Free || camera_mode == CameraMode::Axis)
        {
            _free_camera.set_alignment(camera_mode_to_alignment(camera_mode));
            if (_camera_mode == CameraMode::Orbit)
            {
                _free_camera.set_position(_camera.position());
                _free_camera.set_rotation_yaw(_camera.rotation_yaw());
                _free_camera.set_rotation_pitch(_camera.rotation_pitch());
            }
        }

        _camera_mode = camera_mode;
        _camera_controls->set_mode(camera_mode);
    }

    void Viewer::render_ui()
    {
        _ui_renderer->render(_context);
    }

    void Viewer::render_map()
    {
        ui::Point point = client_cursor_position(_window);
        _map_renderer->set_cursor_position(point);
        _map_renderer->render(_context);
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
            _level->set_selected_room(static_cast<uint16_t>(room));

            _room_navigator->set_selected_room(room);
            _room_navigator->set_room_info(_level->room_info(room));

            _map_renderer->load(_level->room(_level->selected_room()));

            set_camera_mode(CameraMode::Orbit);
        }
    }

    void Viewer::set_alternate_mode(bool enabled)
    {
        if (_level)
        {
            _level->set_alternate_mode(enabled);
            _room_navigator->set_flip(enabled);
        }
    }

    // Resize the window and the rendering system.
    void Viewer::resize()
    {
        // If the window is the same size as before, do nothing.
        Window window{ _window.window() };
        if (window.width() == _window.width() && window.height() == _window.height())
        {
            return;
        }

        // Refresh the window so that the new size is known.
        _window = Window(_window.window());

        _context->ClearState();
        _render_target.reset();

        _swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        create_render_target();
        resize_elements();
    }

    // Create the render target view from the swap chain that has been created.
    void Viewer::create_render_target()
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
        _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
        _render_target = std::make_unique<graphics::RenderTarget>(_device, back_buffer, graphics::RenderTarget::DepthStencilMode::Enabled);
    }

    void Viewer::resize_elements()
    {
        // Inform elements that need to know that the device has been resized.
        _camera.set_view_size(_window.width(), _window.height());
        _free_camera.set_view_size(_window.width(), _window.height());
        _control->set_size(Size(static_cast<float>(_window.width()), static_cast<float>(_window.height())));
        _ui_renderer->set_host_size(_window.width(), _window.height());
        _map_renderer->set_window_size(_window.width(), _window.height());
    }

    // Set up keyboard and mouse input for the camera.
    void Viewer::setup_camera_input()
    {
        using namespace input;

        _mouse.mouse_down += [&](auto button) { _camera_input.mouse_down(button); };
        _mouse.mouse_up += [&](auto button) { _camera_input.mouse_up(button); };
        _mouse.mouse_move += [&](long x, long y) { _camera_input.mouse_move(x, y); };
        _mouse.mouse_wheel += [&](int16_t scroll) { _camera_input.mouse_scroll(scroll); };

        _camera_input.on_rotate += [&](float x, float y)
        {
            ICamera& camera = current_camera();
            const float low_sensitivity = 200.0f;
            const float high_sensitivity = 25.0f;
            const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity) * _camera_sensitivity;
            camera.set_rotation_yaw(camera.rotation_yaw() + x / sensitivity);
            camera.set_rotation_pitch(camera.rotation_pitch() + y / sensitivity);
            if (_level)
            {
                _level->on_camera_moved();
            }
        };

        _camera_input.on_zoom += [&](float zoom)
        {
            _camera.set_zoom(_camera.zoom() + zoom);
            if (_level)
            {
                _level->on_camera_moved();
            }
        };

        _camera_input.on_mode_change += [&](CameraMode mode) { set_camera_mode(mode); };
    }
}
