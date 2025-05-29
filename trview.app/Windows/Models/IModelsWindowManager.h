#pragma once

namespace trview
{
    struct IModelStorage;
    struct IModelsWindow;
    struct IModelsWindowManager
    {
        virtual ~IModelsWindowManager() = 0;
        virtual std::weak_ptr<IModelsWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_model_storage(const std::weak_ptr<IModelStorage>& model_storage) = 0;
        virtual void update(float delta) = 0;
    };
}
