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

    Windows::Windows(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts)
        : MessageHandler(window)
    {
        // TODO: Maybe move somewhere else:
        _token_store += shortcuts->add_shortcut(false, VK_F11) += [&]() { create("Console"); };
        _token_store += shortcuts->add_shortcut(true, 'D') += [&]() { create("Diff"); };
        _token_store += shortcuts->add_shortcut(true, 'I') += [&]() { create("Items"); };
        _token_store += shortcuts->add_shortcut(true, 'K') += [&]() { create("CameraSink"); };
        _token_store += shortcuts->add_shortcut(true, 'L') += [&]() { create("Lights"); };
        _token_store += shortcuts->add_shortcut(true, 'M') += [&]() { create("Rooms"); };
        _token_store += shortcuts->add_shortcut(true, 'P') += [&]() { create("Plugins"); };
        _token_store += shortcuts->add_shortcut(true, 'R') += [&]() { create("Route"); };
        _token_store += shortcuts->add_shortcut(true, 'S') += [&]() { create("Sounds"); };
        _token_store += shortcuts->add_shortcut(true, 'T') += [&]() { create("Triggers"); };
    }

    std::optional<int> Windows::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND)
        {
            switch (LOWORD(wParam))
            {
                case IDM_ABOUT:
                {
                    create("About");
                    break;
                }
                case ID_WINDOWS_CAMERA_SINK:
                {
                    create("CameraSink");
                    break;
                }
                case ID_WINDOWS_CONSOLE:
                {
                    create("Console");
                    break;
                }
                case ID_WINDOWS_DIFF:
                {
                    create("Diff");
                    break;
                }
                case ID_WINDOWS_ITEMS:
                {
                    create("Items");
                    break;
                }
                case ID_WINDOWS_LIGHTS:
                {
                    create("Lights");
                    break;
                }
                case ID_WINDOWS_LOG:
                {
                    create("Log");
                    break;
                }
                case ID_WINDOWS_PACK:
                {
                    create("Pack");
                    break;
                }
                case ID_WINDOWS_PLUGINS:
                {
                    create("Plugins");
                    break;
                }
                case ID_WINDOWS_ROOMS:
                {
                    create("Rooms");
                    break;
                }
                case ID_WINDOWS_ROUTE:
                {
                    create("Route");
                    break;
                }
                case ID_WINDOWS_SOUNDS:
                {
                    create("Sounds");
                    break;
                }
                case ID_WINDOWS_STATICS:
                {
                    create("Statics");
                    break;
                }
                case ID_WINDOWS_TEXTURES:
                {
                    create("Textures");
                    break;
                }
                case ID_WINDOWS_TRIGGERS:
                {
                    create("Triggers");
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

    void Windows::register_window(const std::string& type, const Creator& creator)
    {
        _creators[type] = creator;
    }

    void Windows::render()
    {
        WindowManager<IWindow>::render();
    }

    void Windows::setup(const UserSettings& settings)
    {
        if (settings.camera_sink_startup)
        {
            create("CameraSink");
        }

        if (settings.items_startup)
        {
            create("Items");
        }

        if (settings.rooms_startup)
        {
            create("Rooms");
        }

        if (settings.route_startup)
        {
            create("Sounds");
        }

        if (settings.statics_startup)
        {
            create("Statics");
        }

        if (settings.triggers_startup)
        {
            create("Textures");
        }
    }

    std::vector<std::weak_ptr<IWindow>> Windows::windows(const std::string& type) const
    {
        std::vector<std::weak_ptr<IWindow>> results;
        auto window_list = _windows.find(type);
        if (window_list != _windows.end())
        {
            for (auto& window : window_list->second)
            {
                results.push_back(window.second);
            }
        }
        return results;
    }
}
