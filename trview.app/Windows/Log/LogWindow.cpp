#include "LogWindow.h"

namespace trview
{
    ILogWindow::~ILogWindow()
    {
    }

    LogWindow::LogWindow(const std::shared_ptr<ILog>& log)
        : _log(log)
    {
    }

    LogWindow::~LogWindow()
    {
    }

    void LogWindow::render()
    {
    }
}

