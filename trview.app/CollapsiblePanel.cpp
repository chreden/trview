#include "CollapsiblePanel.h"
#include <trview.ui/StackPanel.h>
#include <trview.ui/Button.h>
#include <trview/resource.h>

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

            HWND init_instance(HWND parent, HINSTANCE hInstance, const std::wstring& window_class, const std::wstring& title, const Size& size, int nCmdShow)
            {
                RECT rect{ 0, 0, static_cast<LONG>(size.width), static_cast<LONG>(size.height) };
                AdjustWindowRect(&rect, window_style, FALSE);

                HWND items_window = CreateWindowW(window_class.c_str(), title.c_str(), window_style,
                    CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, parent, nullptr, hInstance, nullptr);

                ShowWindow(items_window, nCmdShow);
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

            HWND create_window(HWND parent, const std::wstring& window_class, const std::wstring& title, const Size& size)
            {
                HINSTANCE hInstance = GetModuleHandle(nullptr);
                register_class(hInstance, window_class);
                return init_instance(parent, hInstance, window_class, title, size, SW_NORMAL);
            }
        }
    }


    CollapsiblePanel::CollapsiblePanel(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent, const std::wstring& window_class, const std::wstring& title, const Size& size)
        : MessageHandler(create_window(parent, window_class, title, size)), _window_resizer(window()), _device_window(device.create_for_window(window())),
        _ui_renderer(std::make_unique<render::Renderer>(device, shader_storage, font_factory, window().size())),
        _mouse(window()), _keyboard(window())
    {
        _token_store += _window_resizer.on_resize += [=]()
        {
            _device_window->resize();
            update_layout();
            _ui_renderer->set_host_size(window().size());
        };

        _token_store += _keyboard.on_char += [&](auto key) { _ui->process_char(key); };

        _ui = std::make_unique<ui::Window>(Point(), window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f));

        _input = std::make_unique<ui::Input>(window(), *_ui);
    }

    void CollapsiblePanel::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CLOSE)
        {
            on_window_closed();
        }
        else if (message == WM_GETMINMAXINFO)
        {
            RECT rect{ 0, 0, static_cast<LONG>(_ui->size().width), 400 };
            AdjustWindowRect(&rect, window_style, FALSE);

            MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = rect.right - rect.left;
            info->ptMaxTrackSize.x = rect.right - rect.left;
            info->ptMinTrackSize.y = rect.bottom - rect.top;
        }
    }

    void CollapsiblePanel::render(const Device& device, bool vsync)
    {
        _device_window->begin();
        _device_window->clear(DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));
        _ui_renderer->render(device.context());
        _device_window->present(vsync);
    }

    void CollapsiblePanel::set_panels(std::unique_ptr<ui::Control> left_panel, std::unique_ptr<ui::Control> right_panel)
    {
        auto panel = std::make_unique<StackPanel>(Point(), window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(0, 0), StackPanel::Direction::Horizontal, SizeMode::Manual);
        _left_panel = panel->add_child(std::move(left_panel));
        _divider = panel->add_child(create_divider());
        _right_panel = panel->add_child(std::move(right_panel));
        _panels = _ui->add_child(std::move(panel));
        _ui_renderer->load(_ui.get());
    }

    std::unique_ptr<Control> CollapsiblePanel::create_divider()
    {
        return std::make_unique<ui::Window>(Point(), Size(1, window().size().height), Colours::Divider);
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
        auto expander = std::make_unique<Button>(Point(), Size(16, 16), L"<<");
        _token_store += expander->on_click += [this]()
        {
            toggle_expand();
        };
        _expander = parent.add_child(std::move(expander));
    }
}
