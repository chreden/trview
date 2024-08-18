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
        sounds_window->set_level_version(_level_version);
        sounds_window->set_selected_sound_source(_selected_sound_source);
        sounds_window->set_sound_sources(_sound_sources);
        sounds_window->set_sound_storage(_sound_storage);
        sounds_window->on_scene_changed += on_scene_changed;
        sounds_window->on_sound_source_selected += on_sound_source_selected;
        return add_window(sounds_window);
    }

    void SoundsWindowManager::render()
    {
        WindowManager::render();
    }

    void SoundsWindowManager::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
        for (auto& window : _windows)
        {
            window.second->set_level_version(version);
        }
    }

    void SoundsWindowManager::set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source)
    {
        _selected_sound_source = sound_source;
        for (auto& window : _windows)
        {
            window.second->set_selected_sound_source(sound_source);
        }
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
