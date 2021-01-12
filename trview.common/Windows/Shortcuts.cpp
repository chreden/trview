#include "Shortcuts.h"

namespace trview
{
    Shortcuts::Shortcuts(const Window& window)
        : MessageHandler(window), _command_index(41000), _accelerators(nullptr)
    {
    }

    Event<>& Shortcuts::add_shortcut(bool control, uint16_t key)
    {
        uint8_t target_flags = control ? (FVIRTKEY | FCONTROL) : FVIRTKEY;
        for (auto& shortcut : _shortcuts)
        {
            if (shortcut.first.flags == target_flags && shortcut.first.key == key)
            {
                return shortcut.second;
            }
        }

        auto entry = Shortcut{ target_flags, key, _command_index++ };
        _shortcuts.push_back({ entry, Event<>() });

        if (_accelerators)
        {
            create_accelerators();
        }

        return _shortcuts.back().second;
    }

    std::vector<Shortcuts::Shortcut> Shortcuts::shortcuts() const
    {
        std::vector<Shortcut> shortcuts;
        std::transform(_shortcuts.begin(), _shortcuts.end(), std::back_inserter(shortcuts),
            [](const auto& s) { return s.first; });
        return shortcuts;
    }

    void Shortcuts::create_accelerators()
    {
        if (_accelerators)
        {
            DestroyAcceleratorTable(_accelerators);
        }

        std::vector<ACCEL> shortcuts;
        for (const auto& shortcut : _shortcuts)
        {
            shortcuts.push_back({ shortcut.first.flags, shortcut.first.key, shortcut.first.command });
        }
        _accelerators = CreateAcceleratorTable(&shortcuts[0], static_cast<int>(shortcuts.size()));
    }

    void Shortcuts::process_message(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (!_accelerators)
        {
            if (_shortcuts.empty())
            {
                return;
            }
            create_accelerators();
        }

        MSG msg{ window(), message, wParam, lParam, GetTickCount(), 0 };
        if (TranslateAccelerator(window(), _accelerators, &msg))
        {
            return;
        }

        if (message == WM_COMMAND)
        {
            auto id = LOWORD(wParam);
            for (auto& shortcut : _shortcuts)
            {
                if (shortcut.first.command == id)
                {
                    shortcut.second();
                    break;
                }
            }
        }
    }
}
