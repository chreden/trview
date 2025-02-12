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
        void add_level(const std::weak_ptr<ILevel>& level) override;
        std::weak_ptr<ITexturesWindow> create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
    private:
        ITexturesWindow::Source _textures_window_source;
        std::vector<std::weak_ptr<ILevel>> _levels;
    };
}
