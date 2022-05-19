#pragma once

#include <trview.common/Logs/ILog.h>
#include "ILogWindow.h"

namespace trview
{
    class LogWindow final : public ILogWindow
    {
    public:
        explicit LogWindow(const std::shared_ptr<ILog>& log);
        virtual ~LogWindow() = default;
        virtual void render() override;
        virtual void set_number(int32_t number) override;
    private:
        bool render_log_window();

        std::shared_ptr<ILog> _log;
        std::string _id{ "Log 0" };
    };
}
