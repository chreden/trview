#include "SoundsWindowManager.h"
#include "../../Resources/resource.h"
#include "../../Elements/SoundSource/ISoundSource.h"

namespace trview
{
    namespace
    {
        auto find_level(auto&& levels, auto&& level)
        {
            return std::ranges::find_if(levels, [=](auto&& l) { return l.level.lock() == level.lock(); });
        }
    }

    ISoundsWindowManager::~ISoundsWindowManager()
    {
    }

    SoundsWindowManager::SoundsWindowManager(const Window& window, const ISoundsWindow::Source& sounds_window_source)
        : MessageHandler(window), _sounds_window_source(sounds_window_source)
    {
    }

    void SoundsWindowManager::add_level(const std::weak_ptr<ILevel>& level)
    {
        _levels.push_back({ .level = level });
        for (auto& window : _windows)
        {
            window.second->add_level(level);
        }
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
        sounds_window->on_scene_changed += on_scene_changed;
        sounds_window->on_sound_source_selected += on_sound_source_selected;
        for (auto& level : _levels)
        {
            sounds_window->add_level(level.level);
            sounds_window->set_selected_sound_source(level.sound_source);
        }
        return add_window(sounds_window);
    }

    void SoundsWindowManager::render()
    {
        WindowManager::render();
    }

    void SoundsWindowManager::select_sound_source(const std::weak_ptr<ISoundSource>& sound_source)
    {
        if (const auto sound_source_ptr = sound_source.lock())
        {
            const auto found = find_level(_levels, sound_source_ptr->level());
            if (found != _levels.end())
            {
                found->sound_source = sound_source;
                for (auto& window : _windows)
                {
                    window.second->set_selected_sound_source(sound_source);
                }
            }
        }
    }
}
