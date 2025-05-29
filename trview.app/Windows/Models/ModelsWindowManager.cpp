#include "ModelsWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    IModelsWindowManager::~IModelsWindowManager()
    {
    }

    ModelsWindowManager::ModelsWindowManager(const Window& window, const IModelsWindow::Source& meshes_window_source)
        : MessageHandler(window), _meshes_window_source(meshes_window_source)
    {
    }

    std::weak_ptr<IModelsWindow> ModelsWindowManager::create_window()
    {
        auto meshes_window = _meshes_window_source();
        meshes_window->set_model_storage(_model_storage);
        return add_window(meshes_window);
    }

    std::optional<int> ModelsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_MESHES)
        {
            create_window();
        }
        return {};
    }

    void ModelsWindowManager::render()
    {
        WindowManager::render();
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
