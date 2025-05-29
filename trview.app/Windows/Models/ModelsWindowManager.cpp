#include "ModelsWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    IModelsWindowManager::~IModelsWindowManager()
    {
    }

    ModelsWindowManager::ModelsWindowManager(const Window& window, const IModelsWindow::Source& models_window_source)
        : MessageHandler(window), _models_window_source(models_window_source)
    {
    }

    std::weak_ptr<IModelsWindow> ModelsWindowManager::create_window()
    {
        auto models_window = _models_window_source();
        models_window->set_level_texture_storage(_level_texture_storage);
        models_window->set_model_storage(_model_storage);
        return add_window(models_window);
    }

    std::optional<int> ModelsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_MODELS)
        {
            create_window();
        }
        return {};
    }

    void ModelsWindowManager::render()
    {
        WindowManager::render();
    }

    void ModelsWindowManager::set_level_texture_storage(const std::weak_ptr<ILevelTextureStorage>& level_texture_storage)
    {
        _level_texture_storage = level_texture_storage;
        for (auto& window : _windows)
        {
            window.second->set_level_texture_storage(_level_texture_storage);
        }
    }

    void ModelsWindowManager::set_model_storage(const std::weak_ptr<IModelStorage>& model_storage)
    {
        _model_storage = model_storage;
        for (auto& window : _windows)
        {
            window.second->set_model_storage(_model_storage);
        }
    }

    void ModelsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
