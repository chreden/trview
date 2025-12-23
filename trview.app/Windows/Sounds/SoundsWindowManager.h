#pragma once

#include <trview.common/MessageHandler.h>
#include "ISoundsWindowManager.h"
#include "../WindowManager.h"
#include "ISoundsWindow.h"

namespace trview
{
    struct ISoundSource;

    class SoundsWindowManager final : public ISoundsWindowManager, public WindowManager<ISoundsWindow>, public MessageHandler
    {
    public:
        explicit SoundsWindowManager(const Window& window, const ISoundsWindow::Source& sounds_window_source);
        virtual ~SoundsWindowManager() = default;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        std::weak_ptr<ISoundsWindow> create_window() override;
        void render() override;
    private:
        ISoundsWindow::Source _sounds_window_source;
    };
}
