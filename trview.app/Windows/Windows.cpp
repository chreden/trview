#include "Windows.h"
#include "Settings/UserSettings.h"
#include "../Resources/resource.h"
#include "IWindow.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    IWindow::~IWindow()
    {
    }

    IWindows::~IWindows()
    {
    }

    Windows::Windows(
        const Window& window,
        const IWindow::Source& about_window_source,
        const IWindow::Source& camera_sink_window_source,
        const IWindow::Source& console_window_source,
        const IWindow::Source& diff_window_source,
        const IWindow::Source& items_window_source,
        const IWindow::Source& lights_window_source,
        const IWindow::Source& log_window_source,
        const IWindow::Source& pack_window_source,
        const IWindow::Source& plugins_window_source,
        const IWindow::Source& rooms_window_source,
        const IWindow::Source& route_window_source,
        const IWindow::Source& sounds_window_source,
        const IWindow::Source& statics_window_source,
        const IWindow::Source& textures_window_source,
        const IWindow::Source& triggers_window_source,
        const std::shared_ptr<IShortcuts>& shortcuts)
        : MessageHandler(window),
        _about_window_source(about_window_source), _camera_sink_window_source(camera_sink_window_source), _console_window_source(console_window_source),
        _diff_window_source(diff_window_source), _items_window_source(items_window_source), _lights_window_source(lights_window_source),
        _log_window_source(log_window_source), _plugins_window_source(plugins_window_source), _rooms_window_source(rooms_window_source),
        _route_window_source(route_window_source), _sounds_window_source(sounds_window_source), _statics_window_source(statics_window_source),
        _textures_window_source(textures_window_source), _triggers_window_source(triggers_window_source), _pack_window_source(pack_window_source)
    {
        // TODO: Maybe move somewhere else:
        _token_store += shortcuts->add_shortcut(false, VK_F11) += [&]() { add_window(_console_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'D') += [&]() { add_window(_diff_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'I') += [&]() { add_window(_items_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'K') += [&]() { add_window(_camera_sink_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'L') += [&]() { add_window(_lights_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'M') += [&]() { add_window(_rooms_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'P') += [&]() { add_window(_plugins_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'R') += [&]() { add_window(_route_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'S') += [&]() { add_window(_statics_window_source()); };
        _token_store += shortcuts->add_shortcut(true, 'T') += [&]() { add_window(_triggers_window_source()); };
    }

    std::optional<int> Windows::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND)
        {
            switch (LOWORD(wParam))
            {
                case IDM_ABOUT:
                {
                    add_window(_about_window_source());
                    break;
                }
                case ID_WINDOWS_CAMERA_SINK:
                {
                    add_window(_camera_sink_window_source());
                    break;
                }
                case ID_WINDOWS_CONSOLE:
                {
                    add_window(_console_window_source());
                    break;
                }
                case ID_WINDOWS_DIFF:
                {
                    add_window(_diff_window_source());
                    break;
                }
                case ID_WINDOWS_ITEMS:
                {
                    add_window(_items_window_source());
                    break;
                }
                case ID_WINDOWS_LIGHTS:
                {
                    add_window(_lights_window_source());
                    break;
                }
                case ID_WINDOWS_LOG:
                {
                    add_window(_log_window_source());
                    break;
                }
                case ID_WINDOWS_PACK:
                {
                    add_window(_pack_window_source());
                    break;
                }
                case ID_WINDOWS_PLUGINS:
                {
                    add_window(_plugins_window_source());
                    break;
                }
                case ID_WINDOWS_ROOMS:
                {
                    add_window(_rooms_window_source());
                    break;
                }
                case ID_WINDOWS_ROUTE:
                {
                    add_window(_route_window_source());
                    break;
                }
                case ID_WINDOWS_SOUNDS:
                {
                    add_window(_sounds_window_source());
                    break;
                }
                case ID_WINDOWS_STATICS:
                {
                    add_window(_statics_window_source());
                    break;
                }
                case ID_WINDOWS_TEXTURES:
                {
                    add_window(_textures_window_source());
                    break;
                }
                case ID_WINDOWS_TRIGGERS:
                {
                    add_window(_triggers_window_source());
                    break;
                }
            }
        }
        return {};
    }

    std::weak_ptr<IWindow> Windows::create(const std::string& type)
    {
        const auto creator = _creators.find(type);
        if (creator == _creators.end())
        {
            return {};
        }
        return add_window(creator->second());
    }

    void Windows::update(float elapsed)
    {
        WindowManager<IWindow>::update(elapsed);
    }

    void Windows::render()
    {
        WindowManager<IWindow>::render();
    }

    void Windows::setup(const UserSettings& settings)
    {
        if (settings.camera_sink_startup)
        {
            add_window(_camera_sink_window_source());
        }

        if (settings.items_startup)
        {
            add_window(_items_window_source());
        }

        if (settings.rooms_startup)
        {
            add_window(_rooms_window_source());
        }

        if (settings.route_startup)
        {
            add_window(_route_window_source());
        }

        if (settings.statics_startup)
        {
            add_window(_statics_window_source());
        }

        if (settings.triggers_startup)
        {
            add_window(_triggers_window_source());
        }
    }

    std::vector<std::weak_ptr<IWindow>> Windows::windows(const std::string& type) const
    {
        std::vector<std::weak_ptr<IWindow>> results;
        for (const auto& window : _windows)
        {
            if (window.second->type() == type)
            {
                results.push_back(window.second);
            }
        }
        return results;
    }
}
