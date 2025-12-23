#pragma once

#include <trview.common/MessageHandler.h>

#include "../WindowManager.h"
#include "IPackWindowManager.h"
#include "IPackWindow.h"

namespace trview
{
    class PackWindowManager final : public IPackWindowManager, public WindowManager<IPackWindow>, public MessageHandler
    {
    public:
        explicit PackWindowManager(const Window& window, const IPackWindow::Source& pack_window);
        virtual ~PackWindowManager() = default;
        std::weak_ptr<IPackWindow> create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
    private:
        IPackWindow::Source _pack_window_source;
        std::weak_ptr<trlevel::IPack> _pack;
    };
}
