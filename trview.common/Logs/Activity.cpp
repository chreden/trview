#include "Activity.h"
#include "ILog.h"

namespace trview
{
    Activity::Activity(const std::shared_ptr<ILog>& log, const std::string& topic, const std::string& name)
        : _log(log), _topic(topic), _names({ name })
    {
        if (_log)
        {
            _log->log(LogMessage::Status::Information, _topic, _names, "Activity Started");   
        }
    }

    Activity::Activity(const Activity& parent, const std::string& name)
        : _log(parent._log), _topic(parent._topic), _names(parent._names)
    {
        _names.push_back(name);
        if (_log)
        {
            _log->log(LogMessage::Status::Information, _topic, _names, "Activity Started");
        }
    }

    Activity::~Activity()
    {
        if (_log)
        {
            _log->log(LogMessage::Status::Information, _topic, _names, "Activity Ended");
        }
    }

    void Activity::log(const std::string& text) const
    {
        if (_log)
        {
            _log->log(LogMessage::Status::Information, _topic, _names, text);
        }
    }

    void Activity::log(LogMessage::Status status, const std::string& text) const
    {
        if (_log)
        {
            _log->log(status, _topic, _names, text);
        }
    }
}
