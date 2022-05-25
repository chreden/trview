#pragma once

#include <trview.common/Logs/ILog.h>
#include <trview.common/IFiles.h>
#include <trview.common/Windows/IDialogs.h>
#include "ILogWindow.h"

namespace trview
{
    class LogWindow final : public ILogWindow
    {
    public:
        explicit LogWindow(const std::shared_ptr<ILog>& log, const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files);
        virtual ~LogWindow() = default;
        virtual void render() override;
        virtual void set_number(int32_t number) override;
    private:
        bool render_log_window();
        void save_to_file(const std::vector<Message>& messages, int level_offset);

        std::shared_ptr<ILog> _log;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::string _id{ "Log 0" };
    };
}
