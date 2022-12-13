#pragma once

namespace trview
{
    struct ILevelTextureStorage;
    struct ITexturesWindow
    {
        using Source = std::function<std::shared_ptr<ITexturesWindow>()>;

        virtual ~ITexturesWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_texture_storage(const std::shared_ptr<ILevelTextureStorage>& texture_storage) = 0;

        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
    };
}
