#include "CollapsiblePanel.h"
#include <trview.ui/Button.h>
#include <trview.app/Resources/resource.h>
#include <trview.ui.render/Renderer.h>
#include <trview.ui/Layouts/StackLayout.h>

using namespace trview::graphics;
using namespace trview::ui;

namespace trview
{
    namespace
    {
        namespace
        {
            const DWORD window_style = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

            /// Colours commonly used in this class.
            namespace Colours
            {
                const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
            }

            LRESULT CALLBACK panel_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
            {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }

            Window init_instance(const Window& parent, HINSTANCE hInstance, const std::wstring& window_class, const std::wstring& title, const Size& size, int nCmdShow)
            {
                RECT rect{ 0, 0, static_cast<LONG>(size.width), static_cast<LONG>(size.height) };
                AdjustWindowRect(&rect, window_style, FALSE);

                const auto parent_window = is_message_only(parent) ? HWND_MESSAGE : parent.window();
                Window items_window = CreateWindowW(window_class.c_str(), title.c_str(), window_style,
                    CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, parent, nullptr, hInstance, nullptr);

                if (parent_window != HWND_MESSAGE)
                {
                    ShowWindow(items_window, nCmdShow);
                }
                UpdateWindow(items_window);

                return items_window;
            }

            ATOM register_class(HINSTANCE hInstance, const std::wstring& window_class)
            {
                WNDCLASSEXW wcex;
                wcex.cbSize = sizeof(WNDCLASSEX);
                wcex.style = CS_HREDRAW | CS_VREDRAW;
                wcex.lpfnWndProc = panel_window_procedure;
                wcex.cbClsExtra = 0;
                wcex.cbWndExtra = 0;
                wcex.hInstance = hInstance;
                wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRVIEW));
                wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
                wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
                wcex.lpszMenuName = nullptr;
                wcex.lpszClassName = window_class.c_str();
                wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

                return RegisterClassExW(&wcex);
            }

            Window create_window(const Window& parent, const std::wstring& window_class, const std::wstring& title, const Size& size)
            {
                HINSTANCE hInstance = GetModuleHandle(nullptr);
                register_class(hInstance, window_class);
                return init_instance(parent, hInstance, window_class, title, size, SW_NORMAL);
            }
        }
    }

    CollapsiblePanel::CollapsiblePanel(const graphics::IDeviceWindow::Source& device_window_source, std::unique_ptr<ui::render::IRenderer> ui_renderer,
        const Window& parent, const std::wstring& window_class, const std::wstring& title, const ui::IInput::Source& input_source, const Size& size)
        : MessageHandler(create_window(parent, window_class, title, size)), _window_resizer(window()), _device_window(device_window_source(window())),
        _ui_renderer(std::move(ui_renderer)), _parent(parent), _initial_size(size)
    {
        _token_store += _window_resizer.on_resize += [=]()
        {
            _device_window->resize();
            update_layout();
            _ui_renderer->set_host_size(window().size());
        };

        _ui = std::make_unique<ui::Window>(window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f));
        register_change_detection(_ui.get());

        _input = input_source(window(), *_ui);
    }

    void CollapsiblePanel::register_change_detection(Control* control)
    {
        _token_store += control->on_invalidate += [&]() { _ui_changed = true; };
        _token_store += control->on_hierarchy_changed += [&]() { _ui_changed = true; };
        _token_store += control->on_add_child += std::bind(&CollapsiblePanel::register_change_detection, this, std::placeholders::_1);

        // Go through all of the control's children, as they may have been added previously.
        for (auto& child : control->child_elements())
        {
            register_change_detection(child);
        }
    }

    std::optional<int> CollapsiblePanel::process_message(UINT message, WPARAM, LPARAM lParam)
    {
        if (message == WM_CLOSE)
        {
            on_window_closed();
            SetFocus(_parent);
        }
        else if (message == WM_GETMINMAXINFO)
        {
            RECT rect{ 0, 0, static_cast<LONG>(_ui->size().width), static_cast<LONG>(_initial_size.height) };
            AdjustWindowRect(&rect, window_style, FALSE);

            MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = rect.right - rect.left;
            info->ptMaxTrackSize.x = rect.right - rect.left;
            info->ptMinTrackSize.y = rect.bottom - rect.top;
            if (!_allow_increase_height)
            {
                info->ptMaxTrackSize.y = rect.bottom - rect.top;
            }
        }
        return {};
    }

    void CollapsiblePanel::render(bool vsync)
    {
        if (!_ui_changed)
        {
            return;
        }

        _device_window->begin();
        _device_window->clear(DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));
        _ui_renderer->render();
        _device_window->present(vsync);
        _ui_changed = false;
    }

    Control* CollapsiblePanel::root_control() const
    {
        return _ui.get();
    }

    void CollapsiblePanel::set_panels(std::unique_ptr<ui::Control> left_panel, std::unique_ptr<ui::Control> right_panel)
    {
        _panels = _ui->add_child(std::make_unique<ui::Window>(window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f)));
        _panels->set_layout(std::make_unique<StackLayout>(0.0f, StackLayout::Direction::Horizontal, SizeMode::Manual));
        _left_panel = _panels->add_child(std::move(left_panel));
        _divider = _panels->add_child(create_divider());
        _right_panel = _panels->add_child(std::move(right_panel));
        _ui_renderer->load(_ui.get());
    }

    void CollapsiblePanel::set_allow_increase_height(bool value)
    {
        _allow_increase_height = value;
    }

    std::unique_ptr<Control> CollapsiblePanel::create_divider()
    {
        return std::make_unique<ui::Window>(Size(1, window().size().height), Colours::Divider);
    }

    void CollapsiblePanel::update_layout()
    {
        _ui->set_size(window().size());
        _panels->set_size(window().size());
        const auto new_height = window().size().height;
        _left_panel->set_size(Size(_left_panel->size().width, new_height));
        _divider->set_size(Size(_divider->size().width, new_height));
        _right_panel->set_size(Size(_right_panel->size().width, new_height));
    }

    void CollapsiblePanel::toggle_expand()
    {
        _expanded = !_expanded;
        _expander->set_text(_expanded ? L"<<" : L">>");
        _ui->set_size(Size(_expanded ? _right_panel->position().x + _right_panel->size().width : _left_panel->size().width, _ui->size().height));

        // Force resize the window.
        RECT rect{ 0, 0, static_cast<LONG>(_ui->size().width), static_cast<LONG>(_ui->size().height) };
        AdjustWindowRect(&rect, window_style, FALSE);
        SetWindowPos(window(), 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
    }

    void CollapsiblePanel::add_expander(Control& parent)
    {
        set_expander(parent.add_child(std::make_unique<Button>(Size(16, 16), L"<<")));
    }

    void CollapsiblePanel::set_expander(ui::Button* button)
    {
        _expander = button;
        _token_store += _expander->on_click += [this]() { toggle_expand(); };
    }
}
