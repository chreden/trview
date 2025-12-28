#pragma once

#include <trview.common/Logs/ILog.h>
#include <trview.common/IFiles.h>
#include <trview.common/Windows/IDialogs.h>
#include "../IWindow.h"

namespace trview
{
    class LogWindow final : public IWindow
    {
    public:
        struct Names
        {
            const static inline std::string topics_tabs{ "topics" };
            const static inline std::string all_topic{ "All" };
            const static inline std::string save{ "Save" };
        };

        explicit LogWindow(const std::shared_ptr<ILog>& log, const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files);
        virtual ~LogWindow() = default;
        void update(float delta) override;
        void render() override;
        void set_number(int32_t number) override;
        std::string type() const override;
        void receive_message(const Message&) override;
    private:
        bool render_log_window();
        void save_to_file(const std::vector<LogMessage>& messages, int level_offset);

        std::shared_ptr<ILog> _log;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::string _id{ "Log 0" };
    };
}
