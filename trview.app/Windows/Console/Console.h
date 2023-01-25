#pragma once

#include "IConsole.h"

#include <trview.common/Event.h>
#include <trview.common/IFiles.h>

namespace trview
{
    class Console final : public IConsole
    {
    public:
        struct Names
        {
            static const inline std::string log = "##Log";
            static const inline std::string input = "##input";
        };

        explicit Console(const std::shared_ptr<IFiles>& files, const std::shared_ptr<IDialogs>& dialogs);
        virtual ~Console() = default;

        void render() override;
        void set_number(int32_t number) override;
        void set_font(ImFont* font) override;
        void print(const std::string& text) override;
    private:
        static int callback(ImGuiInputTextCallbackData* data);
        bool render_console();

        std::string _buffer;
        std::shared_ptr<IFiles> _files;
        std::shared_ptr<IDialogs> _dialogs;
        std::string _text;
        bool _need_focus{ false };
        bool _go_to_eol{ false };
        ImFont* _font{ nullptr };
        std::string _id{ "Console 0" };
        std::vector<std::string> _recent_files;
        std::vector<std::string> _command_history;
        int32_t _command_history_index{ 0 };
    };
}
