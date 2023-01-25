#include "ConsoleManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    IConsoleManager::~IConsoleManager()
    {
    }

    ConsoleManager::ConsoleManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IConsole::Source& console_source, const std::shared_ptr<IFiles>& files)
        : MessageHandler(window), _console_source(console_source), _files(files)
    {
        _token_store += shortcuts->add_shortcut(false, VK_F11) += [&]() { create_window(); };
    }

    void ConsoleManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<IConsole> ConsoleManager::create_window()
    {
        auto window = _console_source();
        window->on_command += on_command;
        window->set_font(_font);
        return add_window(window);
    }

    std::optional<int> ConsoleManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_CONSOLE)
        {
            create_window();
        }
        return {};
    }

    void ConsoleManager::initialise_ui()
    {
        auto context = ImGui::GetCurrentContext();
        if (context)
        {
            _font = context->IO.Fonts->AddFontFromFileTTF((_files->fonts_directory() + "\\Consola.ttf").c_str(), 14.0f);
        }

        for (auto& window : _windows)
        {
            window.second->set_font(_font);
        }
    }

    void ConsoleManager::print(const std::string& text)
    {
        for (auto& window : _windows)
        {
            window.second->print(text);
        }
    }
}
