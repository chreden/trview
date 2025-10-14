#pragma once

#include "ILog.h"

namespace trview
{
    class Activity
    {
    public:
        /// <summary>
        /// Make a new activity with a root name.
        /// </summary>
        /// <param name="log">The log to write to.</param>
        /// <param name="topic">The topic name.</param>
        /// <param name="name">The activity name.</param>
        explicit Activity(const std::shared_ptr<ILog>& log, const std::string& topic, const std::string& name);
        
        /// <summary>
        /// Create a sub-activity.
        /// </summary>
        /// <param name="parent">The parent activity.</param>
        /// <param name="name">The name for this activity.</param>
        Activity(const Activity& parent, const std::string& name);

        Activity(const Activity&) = delete;
        Activity operator=(const Activity&) = delete;
        ~Activity();
        void log(const std::string& text) const;
        void log(LogMessage::Status status, const std::string& text) const;
    private:
        mutable std::shared_ptr<ILog> _log;
        std::string _topic;
        std::vector<std::string> _names;
    };
}
