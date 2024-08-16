#pragma once

#include <trview.common/MessageHandler.h>
#include "ISoundsWindowManager.h"
#include "../WindowManager.h"
#include "ISoundsWindow.h"

namespace trview
{
    class SoundsWindowManager final : public ISoundsWindowManager, public WindowManager<ISoundsWindow>, public MessageHandler
    {
    public:
        explicit SoundsWindowManager(const Window& window, const ISoundsWindow::Source& sounds_window_source);
        virtual ~SoundsWindowManager() = default;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        std::weak_ptr<ISoundsWindow> create_window() override;
        void render() override;
        void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources) override;
        void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage) override;
    private:
        ISoundsWindow::Source _sounds_window_source;
        std::vector<std::weak_ptr<ISoundSource>> _sound_sources;
        std::weak_ptr<ISoundStorage> _sound_storage;
    };
}
