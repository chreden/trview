#include "LightsWindowManager.h"
#include "../Resources/resource.h"

namespace trview
{
    ILightsWindowManager::~ILightsWindowManager()
    {
    }

    LightsWindowManager::LightsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILightsWindow::Source& lights_window_source)
        : _lights_window_source(lights_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'L') += [&]() { create_window(); };
    }

    std::optional<int> LightsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_LIGHTS)
        {
            create_window();
        }
        return {};
    }

    std::weak_ptr<ILightsWindow> LightsWindowManager::create_window()
    {
        auto lights_window = _lights_window_source();
        return add_window(lights_window);
    }

    void LightsWindowManager::render()
    {
        WindowManager::render();
    }
    
    void LightsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
