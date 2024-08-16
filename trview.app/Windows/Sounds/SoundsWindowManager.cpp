#include "SoundsWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    ISoundsWindowManager::~ISoundsWindowManager()
    {
    }

    SoundsWindowManager::SoundsWindowManager(const Window& window, const ISoundsWindow::Source& sounds_window_source)
        : MessageHandler(window), _sounds_window_source(sounds_window_source)
    {
    }

    std::optional<int> SoundsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_SOUNDS)
        {
            create_window();
        }
        return {};
    }

    std::weak_ptr<ISoundsWindow> SoundsWindowManager::create_window()
    {
        auto sounds_window = _sounds_window_source();
        sounds_window->set_sound_sources(_sound_sources);
        sounds_window->set_sound_storage(_sound_storage);
        return add_window(sounds_window);
    }

    void SoundsWindowManager::render()
    {
        WindowManager::render();
    }

    void SoundsWindowManager::set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources)
    {
        _sound_sources = sound_sources;
        for (auto& window : _windows)
        {
            window.second->set_sound_sources(_sound_sources);
        }
    }

    void SoundsWindowManager::set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage)
    {
        _sound_storage = sound_storage;
        for (auto& window : _windows)
        {
            window.second->set_sound_storage(_sound_storage);
        }
    }
}
