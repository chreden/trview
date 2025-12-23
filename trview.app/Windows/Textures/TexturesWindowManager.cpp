#include "TexturesWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    ITexturesWindowManager::~ITexturesWindowManager()
    {
    }

    TexturesWindowManager::TexturesWindowManager(const Window& window, const ITexturesWindow::Source& textures_window_source)
        : MessageHandler(window), _textures_window_source(textures_window_source)
    {
    }

    std::weak_ptr<ITexturesWindow> TexturesWindowManager::create_window()
    {
        auto window = _textures_window_source();
        return add_window(window);
    }

    std::optional<int> TexturesWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_TEXTURES)
        {
            create_window();
        }
        return {};
    }

    void TexturesWindowManager::render()
    {
        WindowManager::render();
    }
}
