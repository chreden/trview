#include "Activity.h"
#include "ILog.h"

namespace trview
{
    Activity::Activity(const std::shared_ptr<ILog>& log, const std::string& topic, const std::string& name)
        : _log(log), _topic(topic), _name(name)
    {
    }

    void Activity::log(const std::string& text)
    {
        _log->log(_topic, _name, text);
    }
}
