#pragma once

#include <trview.common/MessageHandler.h>

#include "../WindowManager.h"
#include "IModelsWindowManager.h"
#include "IModelsWindow.h"

namespace trview
{
    struct ILevelTextureStorage;
    class ModelsWindowManager final : public IModelsWindowManager, public WindowManager<IModelsWindow>, public MessageHandler
    {
    public:
        explicit ModelsWindowManager(const Window& window, const IModelsWindow::Source& meshes_window_source);
        virtual ~ModelsWindowManager() = default;
        virtual std::weak_ptr<IModelsWindow> create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        void set_model_storage(const std::weak_ptr<IModelStorage>& model_storage) override;
        void update(float delta) override;
    private:
        IModelsWindow::Source _meshes_window_source;
        std::weak_ptr<IModelStorage> _model_storage;
    };
}
