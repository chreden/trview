#pragma once

#include <trview.common/MessageHandler.h>

#include "../WindowManager.h"
#include "ITexturesWindowManager.h"
#include "ITexturesWindow.h"

namespace trview
{
    class TexturesWindowManager final : public ITexturesWindowManager, public WindowManager<ITexturesWindow>, public MessageHandler
    {
    public:
        explicit TexturesWindowManager(const Window& window, const ITexturesWindow::Source& textures_window);
        virtual ~TexturesWindowManager() = default;
        virtual std::weak_ptr<ITexturesWindow> create_window() override;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        virtual void set_texture_storage(const std::shared_ptr<ILevelTextureStorage>& texture_storage) override;
    private:
        ITexturesWindow::Source _textures_window_source;
        std::shared_ptr<ILevelTextureStorage> _texture_storage;
    };
}
