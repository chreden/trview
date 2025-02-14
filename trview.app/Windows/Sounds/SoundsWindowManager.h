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
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void render() override;
        void select_sound_source(const std::weak_ptr<ISoundSource>& sound_source) override;
    private:
        struct Selection
        {
            std::weak_ptr<ILevel> level;
            std::weak_ptr<ISoundSource> sound_source;
        };
        std::vector<Selection> _levels;
        ISoundsWindow::Source _sounds_window_source;
    };
}
