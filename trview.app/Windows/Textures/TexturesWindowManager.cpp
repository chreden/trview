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
        window->set_texture_storage(_texture_storage);
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

    void TexturesWindowManager::set_texture_storage(const std::shared_ptr<ILevelTextureStorage>& texture_storage)
    {
        _texture_storage = texture_storage;
        for (auto& window : _windows)
        {
            window.second->set_texture_storage(_texture_storage);
        }
    }
}
