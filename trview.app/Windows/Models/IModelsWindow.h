#pragma once

namespace trview
{
    struct ILevelTextureStorage;
    struct IModelStorage;
    struct IModelsWindow
    {
        using Source = std::function<std::shared_ptr<IModelsWindow>()>;
        virtual ~IModelsWindow() = 0;
        virtual void render() = 0;
        virtual void set_level_texture_storage(const std::weak_ptr<ILevelTextureStorage>& level_texture_storage) = 0;
        virtual void set_model_storage(const std::weak_ptr<IModelStorage>& model_storage) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void update(float delta) = 0;

        Event<> on_window_closed;
    };
}
