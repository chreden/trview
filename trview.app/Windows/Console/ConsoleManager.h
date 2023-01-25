#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/IFiles.h>
#include "../WindowManager.h"
#include "IConsoleManager.h"

namespace trview
{
    class ConsoleManager final : public IConsoleManager, public WindowManager<IConsole>, public MessageHandler
    {
    public:
        explicit ConsoleManager(const Window& window,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const IConsole::Source& console_source,
            const std::shared_ptr<IFiles>& files);
        virtual ~ConsoleManager() = default;
        void render() override;
        std::weak_ptr<IConsole> create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void initialise_ui() override;
        void print(const std::string& text) override;
    private:
        IConsole::Source _console_source;
        std::shared_ptr<IFiles> _files;
        ImFont* _font{ nullptr };
    };
}
