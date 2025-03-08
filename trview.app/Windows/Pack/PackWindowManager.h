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
        virtual std::weak_ptr<IPackWindow> create_window() override;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        virtual void set_pack(const std::weak_ptr<trlevel::IPack>& pack) override;
    private:
        IPackWindow::Source _pack_window_source;
        std::weak_ptr<trlevel::IPack> _pack;
    };
}
