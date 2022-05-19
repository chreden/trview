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
    private:
        std::shared_ptr<ILog> _log;
    };
}
