#include "Activity.h"
#include "ILog.h"

namespace trview
{
    Activity::Activity(const std::shared_ptr<ILog>& log, const std::string& topic, const std::string& name)
        : _log(log), _topic(topic), _names({ name })
    {
        _log->log(topic, name, "Activity Started");
    }

    Activity::Activity(const Activity& parent, const std::string& name)
        : _log(parent._log), _topic(parent._topic), _names(parent._names)
    {
        _names.push_back(name);
    }

    Activity::~Activity()
    {
        _log->log(_topic, _names, "Activity Ended");
    }

    void Activity::log(const std::string& text)
    {
        _log->log(_topic, _names, text);
    }
}
